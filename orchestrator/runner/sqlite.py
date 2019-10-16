import os
import logging
import argparse
from shutil import copyfile
from injector import run_injector
from .runner import SqlRunner

log = logging.getLogger(__name__)


class SQLiteRunner(SqlRunner):
    def __init__(self, directory: str, inject_delay: int, args: argparse.Namespace):
        super(SQLiteRunner, self).__init__(directory, inject_delay, args)
        self.db_file = os.path.join(directory, 'db.sqlite')

    def init_db(self):
        log.info('Copying sqlite database to a temp file...')
        log.info('Temp file name: ' + self.db_file)
        copyfile('databases/sqlite/tpc-h.sqlite', self.db_file)

    def run_tpch(self, query: int):
        self.process = run_injector(
            os.path.join(self.directory, 'output.txt'),
            'databases/sqlite/queries/{}.sql'.format(query),
            os.path.join(self.directory, 'stderr.txt'),
            self.inject_delay,
            ['databases/sqlite/bin/sqlite3', self.db_file],
            self.fault,
            self.inject_space,
            self.flip_rate,
            self.random_flip_rate
        )

    def clean(self):
        if os.path.exists(self.db_file):
            os.remove(self.db_file)
        pass
