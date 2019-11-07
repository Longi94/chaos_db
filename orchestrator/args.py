import argparse
from queries import *
from db import *
from fault_type import *

parser = argparse.ArgumentParser()
parser.add_argument('-i', '--iterations', type=int, default=1, required=False,
                    help='Number of times to run the experiment')
parser.add_argument('-q', '--query', choices=[TPCH1, TPCH3, SSB11, SSB41, INSERTS, UPDATES], type=str, required=True,
                    help='The TPC-H query to run')
parser.add_argument('-d', '--database', type=str, required=True,
                    choices=[DB_SQLITE, DB_MONETDB, DB_DUCKDB, AHEAD_NORMAL, AHEAD_CONT, AHEAD_CONT_REENC, AHEAD_EARLY,
                             AHEAD_LATE],
                    help='Database to run the experiment on')
parser.add_argument('-dp', '--database-path', type=str, required=False, dest='database_path', default='',
                    help='Path to database resources (binaries, pre-filled dbs)')
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
parser.add_argument('-t', '--threads', required=False, default=1, type=int,
                    help='Run multiple experiments at the same time with this number of threads. For each '
                         'experiment 2 or more processes might be spawned depending on the database.')
parser.add_argument('-s', '--inject-space', dest='inject_space', type=str, choices=['heap', 'stack'],
                    required=False, help='Address space to inject the fault into.')
parser.add_argument('--single', default=False, action='store_true', help='Inject a single fault')
parser.add_argument('-w', '--working-dir', dest='working_directory', type=str, required=True,
                    help='The working directory. This directory will contain all experiment output and artifacts.')
