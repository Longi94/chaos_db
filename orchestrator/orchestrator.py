import argparse
import logging
import os
import time
import signal
from typing import Dict, Set
from multiprocessing.pool import ThreadPool
from functools import partial
from injector import check_injector, init_pool
from db import ResultsDatabase, DB_MONETDB
from monitor import get_monitor
from runner import get_runner
from runner.monetdb import init_mserver5_port_pool
from util import get_hostname, get_dir_name, kill_family
from args import parser

log = logging.getLogger(__name__)


def run(iteration: int, args: argparse.Namespace, experiment_dir: str, existing_iters: Set[int],
        hostname: str) -> Dict or None:
    if iteration in existing_iters:
        log.info('Skipping iteration ' + str(iteration))
        return

    log.info('Iteration ' + str(iteration))
    print('Iteration ' + str(iteration), flush=True)

    db_path = os.path.join(experiment_dir, 'results.sqlite')

    runner = get_runner(args.database[0], iteration, experiment_dir, args, hostname, db_path)
    monitor = get_monitor(args.database[0], args.database_path, iteration, experiment_dir)

    runner.init_db()
    db = None
    try:
        if not runner.serverless:
            runner.start_server()

            monitor.start(args.query)
            runner.run_query(args.query)

            stdout, stderr = runner.query_process.communicate()
            runner.finish_query()

            if len(stdout) == 0:
                stdout = None
            if len(stderr) == 0:
                stderr = None

            runner.server_process.wait()

            monitor.evaluate_query_process(runner.query_process)

            db = ResultsDatabase(db_path, create=False)
            result = db.get_iteration(iteration)
            result.stdout = stdout
            result.stderr = stderr
        else:
            monitor.start(args.query)
            runner.run_query(args.query)

            runner.query_process.wait()
            runner.finish_query()

            db = ResultsDatabase(db_path, create=False)
            result = db.get_iteration(iteration)

        if result is not None:
            monitor.end(result)
            db.commit()

    except Exception as e:
        log.error('Error while running query', exc_info=e)
        if runner.query_process is not None:
            kill_family(runner.query_process.pid, signal.SIGKILL)
        if runner.server_process is not None:
            kill_family(runner.server_process.pid, signal.SIGKILL)

        return None
    finally:
        runner.clean()
        if db is not None:
            db.close()


if __name__ == '__main__':
    parser.add_argument('-v', '--verbose', action='store_true', default=False)
    args = parser.parse_args()

    if args.fault == 'flip' and args.single and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --fault is set to flip and --single is given')

    if args.fault == 'flip' and args.flip_rate is None and not args.single:
        parser.error('--flip-rate is required when --fault is set to flip and --single is not given')

    check_injector(args.debug)
    init_pool(args.threads)

    if args.database[0] == DB_MONETDB:
        init_mserver5_port_pool(args.threads)

    experiment_dir = get_dir_name(
        database=args.database[0],
        query=args.query,
        fault=args.fault,
        inject_to_heap=args.heap,
        inject_to_anon=args.anon,
        inject_to_stack=args.stack,
        flip_rate=args.flip_rate,
        random_flip_rate=args.random_flip_rate,
        suffix=args.suffix
    )
    experiment_dir = os.path.join(args.working_directory, experiment_dir)

    os.makedirs(experiment_dir, exist_ok=True)

    db = ResultsDatabase(os.path.join(experiment_dir, 'results.sqlite'))
    existing = db.get_iterations()
    db.close()

    log_file = os.path.join(experiment_dir, 'experiment.log')

    handlers = [logging.FileHandler(log_file)]

    if args.verbose:
        handlers.append(logging.StreamHandler())

    logging.basicConfig(level=logging.DEBUG,
                        handlers=handlers,
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s] : %(message)s')

    log.info('DB type: ' + args.database[0])
    log.info('Putting everything into ' + experiment_dir)

    thread_count = max(1, args.threads)
    log.info(f'Running on {thread_count} threads')

    start_ts = time.time()
    hostname = get_hostname()

    with ThreadPool(thread_count) as p:
        for _ in p.imap_unordered(
                partial(run, args=args, experiment_dir=experiment_dir, existing_iters=existing,
                        hostname=hostname),
                range(args.iterations)):
            pass

    end_ts = time.time()

    hours, remainder = divmod(end_ts - start_ts, 3600)
    minutes, seconds = divmod(remainder, 60)
    log.info('Finished in {:02}:{:02}:{:02}'.format(int(hours), int(minutes), int(seconds)))
