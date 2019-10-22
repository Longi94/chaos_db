import filecmp
import logging
from subprocess import Popen
from typing import Dict, Any
from time import time

log = logging.getLogger(__name__)


class ProcessMonitor(object):
    def __init__(self, directory: str):
        self.directory = directory
        self.query = None
        self.start_time = None
        self.end_time = None
        self.result = None

        # process completion status
        self.exited = False
        self.return_code = 0
        self.signaled = False
        self.term_sig = 0
        self.max_heap_size = 0
        self.max_stack_size = 0
        self.fault_count = 0

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

        log.info('Monitor done.')

    def start(self, query: str):
        self.query = query
        self.start_time = time()

    def end(self):
        self.end_time = time()
        self.evaluate_result()

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
        }

    def compare_files(self, file1: str, file2: str) -> bool:
        return filecmp.cmp(file1, file2)
