import os
import logging
import subprocess
from shutil import copyfile
from .runner import SqlRunner

log = logging.getLogger(__name__)


class SQLiteRunner(SqlRunner):
    def __init__(self, directory):
        super(SQLiteRunner, self).__init__(directory)
        self.db_file = os.path.join(directory, 'db.sqlite')

    def init_db(self):
        log.info('Creating .sqliterc file')
        with open(os.path.expanduser('~/.sqliterc'), 'w') as f:
            f.write('.headers ON\n')

        log.info('Copying sqlite database to a temp file...')
        log.info('Temp file name: ' + self.db_file)
        copyfile('tpc-h.sqlite', self.db_file)

    def run_tpch(self, query):
        command = ['sqlite3', self.db_file]
        log.info('Running command: ' + ' '.join(command))

        result_dir = os.path.join(self.directory, 'result/sqlite')
        if not os.path.exists(result_dir):
            os.makedirs(result_dir)

        with open('queries/sqlite/{}.sql'.format(query), 'r') as stdin:
            with open(os.path.join(result_dir, 'q{}.out'.format(query)), 'w') as stdout:
                self.process = subprocess.Popen(command, stdout=stdout, stdin=stdin)

    def clean(self):
        pass
