import os
import subprocess
from result import *
from queries import TPCH_UPDATES
from .monitor import ServerlessProcessMonitor
from db import Result

SHA_ORIGINAL = '6fb06d65d5a30e858ed567936c64be5401e3952f'
SHA_UPDATED = 'bd74cffaade77656453c85bcd02952af7b3f0c08'


class SQLiteMonitor(ServerlessProcessMonitor):

    def __init__(self, iteration: int, directory: str, database_dir: str):
        super().__init__(iteration, directory, database_dir, 'sqlite')

    def evaluate_result(self, result: Result):
        super().evaluate_result(result)

        if self.query == TPCH_UPDATES:
            db_file = os.path.join(self.directory, f'db.{self.iteration}.sqlite')
            integrity_stdout = os.path.join(self.directory, 'integrity_stdout.txt')
            integrity_stderr = os.path.join(self.directory, 'integrity_stderr.txt')

            db_hash = subprocess.check_output(['sha1sum', db_file]).decode('utf-8').split(' ')[0]

            with open(os.path.join(self.directory, 'db_hash'), 'w') as f:
                f.write(db_hash)

            # check for corruption
            with open(integrity_stdout, 'w') as f:
                with open(integrity_stderr, 'w') as e:
                    p = subprocess.Popen(
                        [os.path.join(self.database_dir, 'bin/sqlite3'), db_file, 'pragma integrity_check'],
                        stdout=f, stderr=e)

            p.wait()

            if p.returncode != 0:
                result.result = RESULT_DB_CORRUPTED
                return

            with open(integrity_stdout, 'r') as f:
                integrity_check = f.read().strip()

            if integrity_check != 'ok':
                result.result = RESULT_DB_CORRUPTED
                return

            if db_hash == SHA_UPDATED:
                result.result = RESULT_OK
            elif db_hash != SHA_ORIGINAL:
                result.result = RESULT_INCORRECT_OUTPUT
