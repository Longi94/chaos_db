import os
import logging
import argparse
from shutil import copyfile
from injector import run_injector
from .runner import SqlRunner
from queries import *

log = logging.getLogger(__name__)


class SQLiteRunner(SqlRunner):
    def __init__(self, directory: str, args: argparse.Namespace):
        super(SQLiteRunner, self).__init__(directory, args)
        self.db_file = os.path.join(directory, 'db.sqlite')

    def init_db(self):
        log.info('Copying sqlite database to a temp file...')
        log.info('Temp file name: ' + self.db_file)
        copyfile('databases/sqlite/tpc-h.sqlite', self.db_file)

    def run_query(self, query: int):
        query_file = None

        if query == TPCH1:
            query_file = 'databases/sqlite/queries/1.sql'
        elif query == TPCH3:
            query_file = 'databases/sqlite/queries/3.sql'

        if query_file is None:
            raise NameError(f'Unknown query: {query}')

        with open(os.path.join(self.directory, 'inject_stderr.txt'), 'w') as f:
            self.process = run_injector(
                output_file=os.path.join(self.directory, 'output.txt'),
                input_file=query_file,
                error_file=os.path.join(self.directory, 'stderr.txt'),
                child_command=['databases/sqlite/bin/sqlite3', self.db_file],
                fault=self.fault,
                inject_space=self.inject_space,
                flip_rate=self.flip_rate,
                random_flip_rate=self.random_flip_rate,
                mean_runtime=self.mean_runtime,
                inject_stderr=f
            )

    def clean(self):
        if os.path.exists(self.db_file):
            os.remove(self.db_file)
        pass
