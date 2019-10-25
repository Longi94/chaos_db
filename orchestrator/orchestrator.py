import argparse
import logging
import os
import time
from typing import Dict, Set
from multiprocessing.pool import ThreadPool
from functools import partial
from injector import check_injector
from db import ResultsDatabase
from monitor import get_monitor
from runner import get_runner
from util import get_hostname, get_dir_name
from args import parser

log = logging.getLogger(__name__)


def run(iteration: int, args: argparse.Namespace, experiment_dir: str, existing_iters: Set[int]) -> Dict or None:
    if iteration in existing_iters:
        log.info('Skipping iteration ' + str(iteration))
        return

    log.info('Iteration ' + str(iteration))
    print('Iteration ' + str(iteration), flush=True)

    iteration_dir = os.path.join(experiment_dir, str(iteration))
    os.makedirs(iteration_dir, exist_ok=True)

    runner = get_runner(args.database, iteration_dir, args)
    monitor = get_monitor(args.database, iteration_dir)

    runner.init_db()
    try:
        monitor.start(args.query)
        runner.run_query(args.query)
        monitor.monitor(runner.process)
        runner.process.wait()
    except Exception as e:
        log.error('Error while running query', exc_info=e)

    monitor.end()
    runner.clean()

    result = monitor.to_dict()
    result['iteration'] = iteration

    return result


if __name__ == '__main__':
    args = parser.parse_args()

    if args.fault == 'flip' and args.single and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --fault is set to flip and --single is given')

    if args.fault == 'flip' and args.flip_rate is None and not args.single:
        parser.error('--flip-rate is required when --fault is set to flip and --single is not given')

    check_injector()

    experiment_dir = get_dir_name(
        database=args.database,
        query=args.query,
        fault=args.fault,
        inject_space=args.inject_space,
        flip_rate=args.flip_rate,
        random_flip_rate=args.random_flip_rate
    )
    experiment_dir = os.path.join(args.working_directory, experiment_dir)

    os.makedirs(experiment_dir, exist_ok=True)

    db = ResultsDatabase(os.path.join(experiment_dir, 'results.sqlite'))
    existing = db.get_iterations()

    log_file = os.path.join(experiment_dir, 'experiment.log')
    logging.basicConfig(level=logging.DEBUG,
                        handlers=[logging.FileHandler(log_file)],
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s] : %(message)s')

    log.info('DB type: ' + args.database)
    log.info('Putting everything into ' + experiment_dir)

    thread_count = max(1, args.threads)
    log.info(f'Running on {thread_count} threads')

    start_ts = time.time()

    results_file = os.path.join(experiment_dir, 'results.csv')
    resumed = os.path.exists(results_file)
    hostname = get_hostname()

    with ThreadPool(thread_count) as p:
        for result in p.imap_unordered(partial(run, args=args, experiment_dir=experiment_dir,
                                               existing_iters=existing),
                                       range(args.iterations)):
            if result is not None:
                db.insert_result((
                    result['iteration'],
                    hostname,
                    result['result'],
                    result['exited'],
                    result['return_code'],
                    result['signaled'],
                    result['term_sig'],
                    result['runtime'],
                    result['fault_count'],
                    result['max_heap_size'],
                    result['max_stack_size']
                ))

    db.close()

    end_ts = time.time()

    hours, remainder = divmod(end_ts - start_ts, 3600)
    minutes, seconds = divmod(remainder, 60)
    log.info('Finished in {:02}:{:02}:{:02}'.format(int(hours), int(minutes), int(seconds)))
