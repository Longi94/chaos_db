import logging
from subprocess import Popen
from time import time
from result import *
from queries import *
from db import Result

log = logging.getLogger(__name__)


class ProcessMonitor(object):
    def __init__(self, iteration: int, directory: str, database_dir: str):
        self.iteration = iteration
        self.directory = directory
        self.database_dir = database_dir
        self.query = None
        self.start_time = None
        self.end_time = None
        self.result = RESULT_UNKNOWN

        # query process status, if not serverless
        self.query_return_code = 0

    def start(self, query: str):
        self.query = query
        self.start_time = time()

    def end(self, result: Result):
        self.end_time = time()
        self.evaluate_result(result)

    def evaluate_query_process(self, process: Popen):
        self.query_return_code = process.returncode

    def evaluate_result(self, result: Result):
        raise NotImplementedError()

    def compare_output(self, expected: str, output: str):
        with open(expected, 'r') as f:
            expected_str = f.read()
        return expected_str == output


class ServerlessProcessMonitor(ProcessMonitor):

    def __init__(self, iteration: int, directory: str, database_dir: str, db: str):
        super().__init__(iteration, directory, database_dir)
        self.db = db

    def evaluate_result(self, result: Result):
        if result.timeout:
            result.result = RESULT_TIMEOUT
        elif result.exited:
            if result.return_code == 0:
                if self.query in (TPCH1, TPCH3, SSB11, SSB41):
                    answer_file = f'databases/{self.db}/answers'
                    if self.query == TPCH1:
                        answer_file += '/q1.out'
                    elif self.query == TPCH3:
                        answer_file += '/q3.out'
                    elif self.query == SSB11:
                        answer_file += '/ssb11.out'
                    elif self.query == SSB41:
                        answer_file += '/ssb41.out'

                    if self.compare_output(answer_file, result.stdout):
                        result.result = RESULT_OK
                        result.stdout = None
                    else:
                        result.result = RESULT_INCORRECT_OUTPUT
            else:
                result.result = RESULT_ABNORMAL_BEHAVIOR
        elif result.signaled:
            result.result = RESULT_CRASH
