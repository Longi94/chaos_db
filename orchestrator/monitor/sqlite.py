import os
from .monitor import ProcessMonitor
from result import *


class SQLiteMonitor(ProcessMonitor):
    def evaluate_result(self):
        if self.exited:
            if self.return_code == 0:
                if self.compare_files(os.path.join(self.directory, 'output.txt').format(self.query),
                                      'databases/sqlite/answers/q{}.out'.format(self.query)):
                    self.result = RESULT_OK
                else:
                    self.result = RESULT_INCORRECT_OUTPUT
            else:
                self.result = RESULT_ABNORMAL_BEHAVIOR
        elif self.signaled:
            self.result = RESULT_CRASH
        else:
            self.result = RESULT_UNKNOWN
