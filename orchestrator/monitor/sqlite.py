import os
import subprocess
from result import *
from queries import TPCH_UPDATES
from .monitor import ServerlessProcessMonitor

SHA_ORIGINAL = '6fb06d65d5a30e858ed567936c64be5401e3952f'
SHA_UPDATED = 'bd74cffaade77656453c85bcd02952af7b3f0c08'


class SQLiteMonitor(ServerlessProcessMonitor):

    def __init__(self, directory: str):
        super().__init__(directory, 'sqlite')

    def evaluate_result(self):
        super().evaluate_result()

        if self.query == TPCH_UPDATES:
            db_hash = subprocess.check_output(['sha1sum', os.path.join(self.directory, 'db.sqlite')]).decode('utf-8')
            db_hash = db_hash.split(' ')[0]

            if db_hash == SHA_UPDATED:
                self.result = RESULT_OK
            elif db_hash != SHA_ORIGINAL:
                self.result = RESULT_INCORRECT_OUTPUT
