import os
import subprocess
from collect import collect
from args import parser
from util import get_dir_name

if __name__ == '__main__':
    parser.add_argument('-n', '--nodes', type=str, required=True)
    parser.add_argument('-o', '--output-dir', type=str, required=True)
    parser.add_argument('-v', '--values', nargs='+', required=True)
    args = parser.parse_args()

    for db in args.database:
        for val in args.values:
            val = float(val)
            exp_name = get_dir_name(
                database=db,
                query=args.query,
                fault=args.fault,
                inject_to_heap=args.heap,
                inject_to_anon=args.anon,
                inject_to_stack=args.stack,
                flip_rate=val,
                random_flip_rate=args.random_flip_rate
            )

            clush_command = ['clush', '-v', '-w', args.nodes, 'cd', 'chaos_db', '&&', 'PYTHONPATH=./orchestrator',
                             'python3', 'orchestrator/orchestrator.py', '-d', db, '-q', args.query, '-w',
                             args.working_directory, '-i', str(args.iterations), '-t', str(args.threads), '-fr',
                             str(val)]

            if args.database_path is not None:
                clush_command.append('-dp')
                clush_command.append(args.database_path)

            if args.single:
                clush_command.append('--single')

            if args.fault:
                clush_command.append('-f')
                clush_command.append(args.fault)

            if args.mean_runtime is not None:
                clush_command.append('-m')
                clush_command.append(str(args.mean_runtime))

            if args.inject_to_heap:
                clush_command.append('-x')

            if args.inject_to_anon:
                clush_command.append('-y')

            if args.inject_to_stack:
                clush_command.append('-z')

            if args.random_flip_rate:
                clush_command.append('-rfr')

            print(f'Running command: {" ".join(clush_command)}', flush=True)

            process = subprocess.Popen(clush_command)
            process.wait()

            path = os.path.join(args.working_directory, exp_name)
            collect(nodes=args.nodes, path=path, output=os.path.join(args.output_dir, f'{exp_name}_results.sqlite'))
