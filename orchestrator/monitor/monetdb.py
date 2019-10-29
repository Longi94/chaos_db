import os
from .monitor import ProcessMonitor
from result import *
from queries import *


class MonetDBMonitor(ProcessMonitor):
    def evaluate_result(self):
        if self.timeout:
            self.result = RESULT_TIMEOUT
        elif self.signaled:
            if self.term_sig == 9:
                answer_file = None
                if self.query == TPCH1:
                    answer_file = 'databases/monetdb/answers/q1.out'
                elif self.query == TPCH3:
                    answer_file = 'databases/monetdb/answers/q3.out'

                if answer_file is not None and \
                        self.compare_files(os.path.join(self.directory, 'output.txt'), answer_file):
                    self.result = RESULT_OK
                else:
                    self.result = RESULT_INCORRECT_OUTPUT
            else:
                self.result = RESULT_CRASH
        elif self.query_return_code != 0:
            self.result = RESULT_ABNORMAL_BEHAVIOR
        else:
            self.result = RESULT_UNKNOWN
