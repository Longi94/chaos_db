import os
import sqlite3
import subprocess
import argparse
import shutil

TEMP_DIR = 'result-tables'


def collect(nodes: str, path: str, output: str):
    os.makedirs(TEMP_DIR, exist_ok=True)

    result_path = os.path.join(path, 'results.sqlite')

    command = ['echo', '-v', '-w', nodes, '--rcopy', result_path, '--dest', 'result-tables/']
    print(f'Running command, {" ".join(command)}', flush=True)
    process = subprocess.Popen(command)
    process.wait()

    files = sorted(os.listdir(TEMP_DIR))

    if len(files) == 0:
        return

    combined_file = os.path.join(TEMP_DIR, files[0])
    conn = sqlite3.connect(combined_file)

    for db in files[1:]:
        print(f'Merging {db}...')
        db_file = os.path.join(TEMP_DIR, db)
        conn.execute(f'ATTACH \'{db_file}\' AS dba')
        conn.execute('BEGIN')
        conn.execute(f'INSERT INTO result SELECT * FROM dba.result')
        conn.commit()
        conn.execute('DETACH DATABASE dba')

    conn.close()
    os.makedirs(os.path.dirname(output), exist_ok=True)
    shutil.copy(combined_file, output)
    shutil.rmtree('result-tables')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-n', '--nodes', type=str, required=True)
    parser.add_argument('-p', '--path', type=str, required=True)
    parser.add_argument('-o', '--output', type=str, required=True)
    args = parser.parse_args()

    collect(args.nodes, args.path, args.output)
