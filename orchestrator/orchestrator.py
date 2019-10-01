import argparse
import logging
import random
import csv
import os
from typing import Dict
from multiprocessing import Pool
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

    inject_delay = None
    if args.flip:
        inject_delay = int(random.uniform(0.0, args.mean_runtime * 0.75) * 1000)
    runner = get_runner(args.database, iteration_dir, inject_delay)
    monitor = get_monitor(args.database, iteration_dir, inject_delay)

    runner.init_db()
    try:
        monitor.start(args.tpc_h)
        runner.run_tpch(args.tpc_h)
        runner.process.wait()
    except Exception as e:
        log.error('Error while running query', exc_info=e)

    monitor.end()
    runner.clean()

    return monitor.to_dict()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--iterations', type=int, default=1, required=False,
                        help='Number of times to run the experiment')
    parser.add_argument('-t', '--tpc-h', dest='tpc_h', type=int, required=True, help='The TPC-H query to run')
    parser.add_argument('-d', '--database', type=str, choices=[DB_SQLITE], required=True,
                        help='Database to run the experiment on')
    parser.add_argument('-m', '--mean-runtime', dest='mean_runtime', type=float, required=False,
                        help='The expected run time of the query, the time of the bit flip injection (which is random) '
                             'depends on this. Required if --flip is set.')
    parser.add_argument('-f', '--flip', action='store_true', default=False)
    parser.add_argument('-w', '--working-dir', dest='working_directory', type=str, required=True,
                        help='The working directory. This directory will contain all experiment output and artifacts.')
    parser.add_argument('-p', '--processes', required=False, default=1, type=int,
                        help='Run multiple experiments at the same time with this number of processes. For each '
                             'experiment 2 or more processes might be spawned depending on the database.')
    args = parser.parse_args()

    if args.flip and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --flip is set to true')

    if args.flip:
        check_injector()

    logging.basicConfig(level=logging.DEBUG, handlers=[logging.StreamHandler()],
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s:%(process)d] : %(message)s')

    log.info('DB type: ' + args.database)

    results = []

    os.makedirs(args.working_directory, exist_ok=True)
    log.info('Putting everything into ' + args.working_directory)

    if args.processes > 1:
        with Pool(args.processes) as p:
            for result in p.imap(partial(run, args=args), range(args.iterations)):
                results.append(result)
    else:
        for i in range(args.iterations):
            results.append(run(i, args))

    with open(os.path.join(args.working_directory, 'results.csv'), mode='w') as output_csv:
        if len(results) > 0:
            writer = csv.writer(output_csv, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

            writer.writerow(list(results[0].keys()))
            for result in results:
                writer.writerow(list(result.values()))
