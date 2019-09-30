import os
from .monitor import ProcessMonitor
from result import *


class SQLiteMonitor(ProcessMonitor):
    def evaluate_result(self):
        rc_file = os.path.join(self.directory, 'rc')
        if not os.path.exists(rc_file):
            # TODO what happened?
            return

        with open(rc_file, 'r') as f:
            self.return_code = int(f.read().strip())

        if self.return_code == 0:
            if self.compare_files(os.path.join(self.directory, 'output.txt').format(self.query),
                                  'databases/sqlite/answers/q{}.out'.format(self.query)):
                self.result = RESULT_OK
            else:
                self.result = RESULT_INCORRECT_OUTPUT
        else:
            # TODO
            pass
