import argparse
import logging
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
    args = parser.parse_args()

    logging.basicConfig(level=logging.DEBUG, handlers=[logging.StreamHandler()],
                        format='%(asctime)s %(levelname)7s %(name)s [%(threadName)s] : %(message)s')

    log.info('DB type: ' + args.database)

    runner = get_runner(args.database)
    monitor = get_monitor(args.database)

    runner.init_db()
    try:
        runner.run_tpch(args.tpc_h)
        monitor.start(runner.process, args.tpc_h)
        runner.process.wait()
    except:
        pass
    monitor.end()
    runner.clean()
