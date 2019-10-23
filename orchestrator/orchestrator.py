import argparse
import logging
import os
import time
from typing import Dict, Set
from multiprocessing.pool import ThreadPool
from functools import partial
from injector import check_injector
from db import DB_SQLITE, ResultsDatabase
from monitor import get_monitor
from runner import get_runner
from queries import *
from fault_type import *
from util import get_hostname

log = logging.getLogger(__name__)


def run(iteration: int, args: argparse.Namespace, experiment_dir: str, existing_iters: Set[int]) -> Dict or None:
    log.info('Iteration ' + str(iteration))
    print('Iteration ' + str(iteration))

    if iteration in existing_iters:
        log.info('Skipping iteration ' + str(iteration))
        return

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


def get_dir_name(args: argparse.Namespace) -> str:
    name = f'{args.database}_{args.query}'

    if args.fault is not None:
        name += f'_{args.fault}'

    if args.fault == FAULT_BIT_FLIP:
        name += f'_{args.flip_rate}'

        if args.random_flip_rate:
            name += '_randomized'

    return name


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--iterations', type=int, default=1, required=False,
                        help='Number of times to run the experiment')
    parser.add_argument('-q', '--query', choices=[TPCH1, TPCH3, INSERTS, UPDATES], type=str, required=True,
                        help='The TPC-H query to run')
    parser.add_argument('-d', '--database', type=str, choices=[DB_SQLITE], required=True,
                        help='Database to run the experiment on')
    parser.add_argument('-m', '--mean-runtime', dest='mean_runtime', type=int, required=False,
                        help='The expected run time of the query in milliseconds, the time of the bit flip injection '
                             '(which is random) depends on this. Required if --flip is set.')
    parser.add_argument('-f', '--fault', type=str, choices=[FAULT_BIT_FLIP, FAULT_STUCK_BIT],
                        help='The type of fault to inject.')
    parser.add_argument('-fr', '--flip-rate', type=float, dest='flip_rate', default=1.0,
                        help='Frequency of bit-flips in a bit/second/megabytes unit. Required if the fault type is '
                             '"flip".')
    parser.add_argument('-rfr', '--random-flip-rate', default=False, action='store_true', dest='random_flip_rate',
                        help='Randomize the frequency of bit flips keeping the flip-rate in mind. At least one flip is '
                             'ensured if --mean-runtime is provided.')
    parser.add_argument('-w', '--working-dir', dest='working_directory', type=str, required=True,
                        help='The working directory. This directory will contain all experiment output and artifacts.')
    parser.add_argument('-t', '--threads', required=False, default=1, type=int,
                        help='Run multiple experiments at the same time with this number of threads. For each '
                             'experiment 2 or more processes might be spawned depending on the database.')
    parser.add_argument('-s', '--inject-space', dest='inject_space', type=str, choices=['heap', 'stack'],
                        required=False, help='Address space to inject the fault into.')
    parser.add_argument('--single', default=False, action='store_true', help='Inject a single fault')
    args = parser.parse_args()

    if args.fault == 'flip' and args.single and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --fault is set to flip and --single is given')

    if args.fault == 'flip' and args.flip_rate is None and not args.single:
        parser.error('--flip-rate is required when --fault is set to flip and --single is not given')

    check_injector()

    experiment_dir = get_dir_name(args)
    experiment_dir = os.path.join(args.working_directory, experiment_dir)

    os.makedirs(experiment_dir, exist_ok=True)

    db = ResultsDatabase(os.path.join(experiment_dir, 'results.sqlite'))
    existing = db.get_iterations()

    log_file = os.path.join(experiment_dir, 'experiment.log')
    logging.basicConfig(level=logging.DEBUG,
                        handlers=[logging.FileHandler(log_file, mode='w')],
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
