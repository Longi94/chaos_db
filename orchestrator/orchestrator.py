import argparse
import logging
import tempfile
import random
from time import sleep
from db import *
from monitor import get_monitor
from runner import get_runner
from injector import inject_bit_flip

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
    args = parser.parse_args()

    if args.flip and args.mean_runtime is None:
        parser.error('--mean-runtime is required when --flip is set to true')

    logging.basicConfig(level=logging.DEBUG, handlers=[logging.StreamHandler()],
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s] : %(message)s')

    log.info('DB type: ' + args.database)

    with tempfile.TemporaryDirectory() as temp_dir:
        log.info('Temp dir: ' + temp_dir)
        runner = get_runner(args.database, temp_dir)
        monitor = get_monitor(args.database, temp_dir)

        runner.init_db()
        try:
            runner.run_tpch(args.tpc_h)
            monitor.start(runner.process, args.tpc_h)

            if args.flip:
                inject_delay = random.uniform(0.0, args.mean_runtime * 0.75)
                log.info('Will inject bit-flip in {} seconds'.format(inject_delay))
                sleep(inject_delay)
                monitor.set_inject_time()
                inject_bit_flip(runner.process.pid)

            runner.process.wait()
        except Exception as e:
            log.error('Error while running query', exc_info=e)

        monitor.end()
        runner.clean()

    monitor.print_result()
