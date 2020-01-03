from .monitor import ProcessMonitor
from result import *
from queries import *
from db import Result


class MonetDBMonitor(ProcessMonitor):
    def evaluate_result(self, result: Result):
        if result.timeout:
            result.result = RESULT_TIMEOUT
        elif result.signaled:
            if result.term_sig == 9:
                answer_file = None
                if self.query == TPCH1:
                    answer_file = 'databases/monetdb/answers/q1.out'
                elif self.query == TPCH3:
                    answer_file = 'databases/monetdb/answers/q3.out'

                if self.compare_output(answer_file, result.stdout):
                    result.result = RESULT_OK
                    result.stdout = None
                else:
                    result.result = RESULT_INCORRECT_OUTPUT
            else:
                result.result = RESULT_CRASH
        elif self.query_return_code != 0:
            result.result = RESULT_ABNORMAL_BEHAVIOR
        else:
            result.result = RESULT_UNKNOWN
