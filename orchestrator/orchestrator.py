import argparse
import logging
import random
import csv
import os
from typing import Dict
from multiprocessing.pool import ThreadPool
from functools import partial
from injector import check_injector
from db import *
from monitor import get_monitor
from runner import get_runner

log = logging.getLogger(__name__)


def run(iteration: int, args: argparse.Namespace) -> Dict:
    log.info('Iteration ' + str(iteration))

    iteration_dir = os.path.join(args.working_directory, str(iteration))
    os.makedirs(iteration_dir, exist_ok=True)

    runner = get_runner(args.database, iteration_dir, args)
    monitor = get_monitor(args.database, iteration_dir)

    runner.init_db()
    try:
        monitor.start(args.tpc_h)
        runner.run_tpch(args.tpc_h)
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
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--iterations', type=int, default=1, required=False,
                        help='Number of times to run the experiment')
    parser.add_argument('-q', '--query', dest='tpc_h', type=int, required=True, help='The TPC-H query to run')
    parser.add_argument('-d', '--database', type=str, choices=[DB_SQLITE], required=True,
                        help='Database to run the experiment on')
    parser.add_argument('-m', '--mean-runtime', dest='mean_runtime', type=int, required=False,
                        help='The expected run time of the query in milliseconds, the time of the bit flip injection '
                             '(which is random) depends on this. Required if --flip is set.')
    parser.add_argument('-f', '--fault', type=str, choices=['flip', 'stuck'],
                        help='The type of fault to inject.')
    parser.add_argument('-fr', '--flip-rate', type=float, dest='flip_rate',
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
    args = parser.parse_args()

    if args.fault == 'flip' and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --fault is set to flip')

    if args.fault == 'flip' and args.flip_rate is None:
        parser.error('--flip-rate is required when --fault is set to flip')

    if args.fault is not None:
        check_injector()

    os.makedirs(args.working_directory, exist_ok=True)

    log_file = os.path.join(args.working_directory, 'experiment.log')
    logging.basicConfig(level=logging.DEBUG,
                        handlers=[logging.StreamHandler(), logging.FileHandler(log_file, mode='w')],
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s] : %(message)s')

    log.info('DB type: ' + args.database)
    log.info('Putting everything into ' + args.working_directory)

    thread_count = max(1, args.threads)
    log.info(f'Running on {thread_count} threads')

    with open(os.path.join(args.working_directory, 'results.csv'), mode='w') as output_csv:
        writer = csv.writer(output_csv, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

        writer.writerow([
            'iteration',
            'result',
            'exited',
            'return_code',
            'signaled',
            'term_sig',
            'runtime',
            'fault_count',
            'max_heap_size',
            'max_stack_size'
        ])
        output_csv.flush()

        with ThreadPool(thread_count) as p:
            for result in p.imap_unordered(partial(run, args=args), range(args.iterations)):
                writer.writerow([
                    result['iteration'],
                    result['result'],
                    result['exited'],
                    result['return_code'],
                    result['signaled'],
                    result['term_sig'],
                    result['runtime'],
                    result['fault_count'],
                    result['max_heap_size'],
                    result['max_stack_size']
                ])
                output_csv.flush()
