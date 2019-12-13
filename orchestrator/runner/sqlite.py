import os
import logging
import argparse
from shutil import copyfile
from injector import run_injector
from .runner import SqlRunner
from queries import *

log = logging.getLogger(__name__)


class SQLiteRunner(SqlRunner):
    def __init__(self, directory: str, args: argparse.Namespace, iteration: int, hostname: str, results_db: str):
        super(SQLiteRunner, self).__init__(directory, args, iteration, hostname, results_db)
        self.db_file = os.path.join(directory, 'db.sqlite')
        self.db_journal = os.path.join(directory, 'db.sqlite-journal')
        self.serverless = True

    def init_db(self):
        log.info('Copying sqlite database to a temp file...')
        log.info('Temp file name: ' + self.db_file)
        copyfile(os.path.join(self.database_dir, 'tpc-h.sqlite'), self.db_file)

    def run_query(self, query: int):
        query_file = None

        if query == TPCH1:
            query_file = 'databases/sqlite/queries/1.sql'
        elif query == TPCH3:
            query_file = 'databases/sqlite/queries/3.sql'
        elif query == TPCH_UPDATES:
            query_file = 'databases/sqlite/update.sql.tmp'

        if query_file is None:
            raise NameError(f'Unknown query: {query}')

        with open(os.path.join(self.directory, 'inject_stderr.txt'), 'w') as f:
            self.query_process = run_injector(
                input_file=query_file,
                database=self.results_db,
                iteration=self.iteration,
                hostname=self.hostname,
                child_command=[os.path.join(self.database_dir, 'bin/sqlite3'), self.db_file],
                fault=self.fault,
                inject_space=self.inject_space,
                flip_rate=self.flip_rate,
                random_flip_rate=self.random_flip_rate,
                mean_runtime=self.mean_runtime,
                inject_stderr=f,
                single=self.single
            )

    def clean(self):
        if os.path.exists(self.db_file):
            os.remove(self.db_file)
        if os.path.exists(self.db_journal):
            os.remove(self.db_journal)
