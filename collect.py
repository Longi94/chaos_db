import os
import sqlite3
import subprocess
import argparse
import shutil

TEMP_DIR = 'result-tables'

parser = argparse.ArgumentParser()
parser.add_argument('-n', '--nodes', type=str, required=True)
parser.add_argument('-p', '--path', type=str, required=True)
parser.add_argument('-o', '--output', type=str, required=True)
args = parser.parse_args()

os.makedirs(TEMP_DIR, exist_ok=True)

result_path = os.path.join(args.path, 'results.sqlite')

process = subprocess.Popen(['clush', '-v', '-w', args.nodes, '--rcopy', result_path, '--dest', 'result-tables/'])
process.wait()

files = sorted(os.listdir(TEMP_DIR))

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
shutil.copy(combined_file, args.output)
shutil.rmtree('result-tables')
