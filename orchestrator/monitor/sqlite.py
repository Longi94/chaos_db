import os
from .monitor import ProcessMonitor
from result import *
from queries import *


class SQLiteMonitor(ProcessMonitor):
    def evaluate_result(self):
        if self.timeout:
            self.result = RESULT_TIMEOUT
        elif self.exited:
            if self.return_code == 0:
                answer_file = None
                if self.query == TPCH1:
                    answer_file = 'databases/sqlite/answers/q1.out'
                elif self.query == TPCH3:
                    answer_file = 'databases/sqlite/answers/q3.out'

                if answer_file is not None and \
                        self.compare_files(os.path.join(self.directory, 'output.txt'), answer_file):
                    self.result = RESULT_OK
                else:
                    self.result = RESULT_INCORRECT_OUTPUT
            else:
                self.result = RESULT_ABNORMAL_BEHAVIOR
        elif self.signaled:
            self.result = RESULT_CRASH
        else:
            self.result = RESULT_UNKNOWN
