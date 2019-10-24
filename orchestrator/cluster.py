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

    for val in args.values:
        exp_name = get_dir_name(
            database=args.database,
            query=args.query,
            fault=args.fault,
            inject_space=args.inject_space,
            flip_rate=val,
            random_flip_rate=args.random_flip_rate
        )

        clush_command = ['clush', '-v', 'w', args.nodes, 'cd', 'chaos_db', '&&', 'PYTHONPATH=./orchestrator', 'python3',
                         'orchestrator/orchestrator.py', '-d', args.database, '-q', args.query, '-w', '~/experiments/',
                         '-i', str(args.iterations), '-t', str(args.threads), '-fr', str(args.flip_rate)]

        if args.single:
            clush_command.append('--single')

        if args.fault:
            clush_command.append('-f')
            clush_command.append(args.fault)

        if args.mean_runtime is not None:
            clush_command.append('-m')
            clush_command.append(str(args.mean_runtime))

        if args.inject_space:
            clush_command.append('-s')
            clush_command.append(args.inject_space)

        if args.random_flip_rate:
            clush_command.append('-rfr')

        print(f'Running command: {" ".join(clush_command)}', flush=True)

        process = subprocess.Popen(clush_command)
        process.wait()

        collect(nodes=args.nodes, path=f'~/experiments/{exp_name}',
                output=os.path.join(args.output_dir, f'{exp_name}_results.sqlite'))
