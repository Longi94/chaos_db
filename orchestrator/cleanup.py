import os
from multiprocessing import Pool
from db import ResultsDatabase
from shutil import rmtree
from result import *
from sqlalchemy.exc import OperationalError


def clean_exp(exp_name):
    if not os.path.isdir(exp_name):
        return
    print(f'Cleaning {exp_name}')

    db = exp_name.split('_')[0]
    db_name = os.path.join(exp_name, 'results.sqlite')

    if not os.path.exists(db_name):
        return

    results_db = ResultsDatabase(db_name)

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN timeout INTEGER''')
        print('Added column timeout')
    except OperationalError:
        print('Column timeout already exists')

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN stdout BLOB''')
        print('Added column stdout')
    except OperationalError:
        print('Column stdout already exists')

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN stderr BLOB''')
        print('Added column stderr')
    except OperationalError:
        print('Column stderr already exists')

    try:
        results_db.engine.execute('''ALTER TABLE result ADD COLUMN inject_stderr BLOB''')
        print('Added column inject_stderr')
    except OperationalError:
        print('Column inject_stderr already exists')

    results = results_db.get_results()

    for row in results:
        iteration_dir = os.path.join(exp_name, str(row.iteration))

        if not os.path.exists(iteration_dir):
            continue

        print(f'Cleaning {iteration_dir}')

        if row.result == RESULT_OK:
            # remove dir, everything in there is useless
            rmtree(iteration_dir)
            continue

        if row.result == RESULT_TIMEOUT:
            os.remove(os.path.join(iteration_dir, 'output.txt'))

        try:
            with open(os.path.join(iteration_dir, 'inject_stderr.txt'), 'rb') as f:
                row.inject_stderr = f.read()
        except FileNotFoundError:
            pass

        try:
            with open(os.path.join(iteration_dir, 'output.txt'), 'rb') as f:
                row.stdout = f.read()
        except FileNotFoundError:
            pass

        try:
            with open(os.path.join(iteration_dir, 'stderr.txt'), 'rb') as f:
                row.stderr = f.read()
        except FileNotFoundError:
            pass

        results_db.commit()

        rmtree(iteration_dir)

    results_db.close()


with Pool(processes=5) as p:
    for _ in p.imap_unordered(clean_exp, os.listdir('.')):
        pass
