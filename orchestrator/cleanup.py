import os
from multiprocessing import Pool
from db import ResultsDatabase
from shutil import rmtree
from result import *
from sqlalchemy.exc import OperationalError


def clean_exp(exp_name):
    if not os.path.isdir(exp_name):
        return
    print(f'Cleaning {exp_name}', flush=True)

    db = exp_name.split('_')[0]
    db_name = os.path.join(exp_name, 'results.sqlite')

    if not os.path.exists(db_name):
        return

    results_db = ResultsDatabase(db_name)

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN timeout INTEGER''')
    except OperationalError:
        pass

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN stdout BLOB''')
    except OperationalError:
        pass

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN stderr BLOB''')
    except OperationalError:
        pass

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN inject_stderr BLOB''')
    except OperationalError:
        pass

    results = results_db.get_results()

    for row in results:
        iteration_dir = os.path.join(exp_name, str(row.iteration))

        if not os.path.exists(iteration_dir):
            continue

        if row.result == RESULT_OK:
            # remove dir, everything in there is useless
            rmtree(iteration_dir)
            continue

        if os.path.exists(os.path.join(iteration_dir, 'output.txt')):
            if row.result != RESULT_TIMEOUT:
                with open(os.path.join(iteration_dir, 'output.txt'), 'rb') as f:
                    row.stdout = f.read()

        if os.path.exists(os.path.join(iteration_dir, 'inject_stderr.txt')):
            with open(os.path.join(iteration_dir, 'inject_stderr.txt'), 'rb') as f:
                row.inject_stderr = f.read()

        if os.path.exists(os.path.join(iteration_dir, 'stderr.txt')):
            with open(os.path.join(iteration_dir, 'stderr.txt'), 'rb') as f:
                row.stderr = f.read()

        results_db.commit()

        rmtree(iteration_dir)

    results_db.close()


with Pool(processes=5) as p:
    for _ in p.imap_unordered(clean_exp, os.listdir('.')):
        pass
