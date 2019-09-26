import os
import logging
from shutil import copyfile
from injector import run_injector
from .runner import SqlRunner

log = logging.getLogger(__name__)


class SQLiteRunner(SqlRunner):
    def __init__(self, directory, inject_delay):
        super(SQLiteRunner, self).__init__(directory, inject_delay)
        self.db_file = os.path.join(directory, 'db.sqlite')

    def init_db(self):
        log.info('Copying sqlite database to a temp file...')
        log.info('Temp file name: ' + self.db_file)
        copyfile('databases/sqlite/tpc-h.sqlite', self.db_file)

    def run_tpch(self, query):
        result_dir = os.path.join(self.directory, 'result/sqlite')
        if not os.path.exists(result_dir):
            os.makedirs(result_dir)

        self.process = run_injector(
            os.path.join(result_dir, 'q{}.out'.format(query)),
            'databases/sqlite/queries/{}.sql'.format(query),
            self.inject_delay,
            ['databases/sqlite/bin/sqlite3', self.db_file]
        )

    def clean(self):
        pass
