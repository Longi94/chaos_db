import os
import filecmp
import logging
from subprocess import Popen
from typing import Dict, Any
from time import time
from result import *
from queries import *

log = logging.getLogger(__name__)


class ProcessMonitor(object):
    def __init__(self, directory: str, database_dir: str):
        self.directory = directory
        self.database_dir = database_dir
        self.query = None
        self.start_time = None
        self.end_time = None
        self.result = RESULT_UNKNOWN

        # injector process completion status
        self.exited = False
        self.return_code = 0
        self.signaled = False
        self.term_sig = 0
        self.max_heap_size = 0
        self.max_stack_size = 0
        self.fault_count = 0
        self.timeout = False
        self.detected = False

        # query process status, if not serverless
        self.query_return_code = 0

    def monitor(self, process: Popen):
        def get_val(line: str):
            return line.split(':')[1].strip()

        for line in process.stdout:
            line = line.decode("utf-8").strip()
            log.info(line)

            if line.startswith('WIFEXITED: '):
                self.exited = get_val(line) == '1'
            if line.startswith('WEXITSTATUS: '):
                self.return_code = int(get_val(line))
            if line.startswith('WIFSIGNALED: '):
                self.signaled = get_val(line) == '1'
            if line.startswith('WTERMSIG: '):
                self.term_sig = int(get_val(line))
            if line.startswith('MAX_HEAP_SIZE: '):
                self.max_heap_size = int(get_val(line))
            if line.startswith('MAX_STACK_SIZE: '):
                self.max_stack_size = int(get_val(line))
            if line.startswith('FAULT_COUNT: '):
                self.fault_count = int(get_val(line))
            if line.startswith('TIMEOUT: '):
                self.timeout = get_val(line) == '1'

        log.info('Monitor done.')

    def start(self, query: str):
        self.query = query
        self.start_time = time()

    def end(self):
        self.end_time = time()
        self.evaluate_result()

    def evaluate_query_process(self, process: Popen):
        self.query_return_code = process.returncode

    def evaluate_result(self):
        raise NotImplementedError()

    def to_dict(self) -> Dict[str, Any]:
        return {
            'runtime': self.end_time - self.start_time,
            'return_code': self.return_code,
            'result': self.result,
            'exited': self.exited,
            'signaled': self.signaled,
            'term_sig': self.term_sig,
            'max_heap_size': self.max_heap_size,
            'max_stack_size': self.max_stack_size,
            'fault_count': self.fault_count,
            'detected': self.detected
        }

    def compare_files(self, file1: str, file2: str) -> bool:
        return filecmp.cmp(file1, file2)


class ServerlessProcessMonitor(ProcessMonitor):

    def __init__(self, directory: str, database_dir: str, db: str):
        super().__init__(directory, database_dir)
        self.db = db

    def evaluate_result(self):
        if self.timeout:
            self.result = RESULT_TIMEOUT
        elif self.exited:
            if self.return_code == 0:
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

                    if self.compare_files(os.path.join(self.directory, 'output.txt'), answer_file):
                        self.result = RESULT_OK
                    else:
                        self.result = RESULT_INCORRECT_OUTPUT
            else:
                self.result = RESULT_ABNORMAL_BEHAVIOR
        elif self.signaled:
            self.result = RESULT_CRASH
