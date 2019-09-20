import os
import logging
import tempfile
import subprocess
from shutil import copyfile
from .runner import SqlRunner

log = logging.getLogger(__name__)


class SQLiteRunner(SqlRunner):
    def __init__(self):
        self.db_file = None
        self.process = None

    def init_db(self):
        log.info('Creating .sqliterc file')
        with open(os.path.expanduser('~/.sqliterc'), 'w') as f:
            f.write('.headers ON\n')

        log.info('Copying sqlite database to a temp file...')
        self.db_file = tempfile.NamedTemporaryFile(delete=False)
        log.info('Temp file name: ' + self.db_file.name)
        copyfile('tpc-h.sqlite', self.db_file.name)

    def run_tpch(self, query):
        command = ['sqlite3', self.db_file.name]
        log.info('Running command: ' + ' '.join(command))

        if not os.path.exists('result/sqlite'):
            os.makedirs('result/sqlite')

        with open('queries/sqlite/{}.sql'.format(query), 'r') as stdin:
            with open('result/sqlite/q{}.out'.format(query), 'w') as stdout:
                self.process = subprocess.Popen(command, stdout=stdout, stdin=stdin)

        return self.process.pid

    def clean(self):
        log.info('Removing temp database file...')
        os.unlink(self.db_file.name)
        self.db_file.close()
