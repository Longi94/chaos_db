import argparse
import logging
import tempfile
import random
import csv
from db import *
from monitor import get_monitor
from runner import get_runner

log = logging.getLogger(__name__)

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
    parser.add_argument('-o', '--output', type=str, required=True, help='Output CSV to save results to.')
    args = parser.parse_args()

    if args.flip and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --flip is set to true')

    logging.basicConfig(level=logging.DEBUG, handlers=[logging.StreamHandler()],
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s] : %(message)s')

    log.info('DB type: ' + args.database)

    results = []

    for i in range(args.iterations):
        log.info('Iteration ' + str(i))
        with tempfile.TemporaryDirectory() as temp_dir:
            log.info('Temp dir: ' + temp_dir)

            inject_delay = None
            if args.flip:
                inject_delay = int(random.uniform(0.0, args.mean_runtime * 0.75) * 1000)
            runner = get_runner(args.database, temp_dir, inject_delay)
            monitor = get_monitor(args.database, temp_dir, inject_delay)

            runner.init_db()
            try:
                monitor.start(args.tpc_h)
                runner.run_tpch(args.tpc_h)
                runner.process.wait()
            except Exception as e:
                log.error('Error while running query', exc_info=e)

            monitor.end()
            runner.clean()

        results.append(monitor.to_dict())

    with open(args.output, mode='w') as output_csv:
        if len(results) > 0:
            writer = csv.writer(output_csv, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

            writer.writerow(list(results[0].keys()))
            for result in results:
                writer.writerow(list(result.values()))
