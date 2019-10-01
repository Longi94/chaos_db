import filecmp
from typing import Dict, Any
from time import time


class ProcessMonitor(object):
    def __init__(self, directory: str, inject_delay: int):
        self.directory = directory
        self.query = None
        self.start_time = None
        self.end_time = None
        self.inject_delay = inject_delay
        self.return_code = None
        self.result = None

    def start(self, query: int):
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
            'inject_delay': self.inject_delay,
            'result': self.result
        }

    def compare_files(self, file1: str, file2: str) -> bool:
        return filecmp.cmp(file1, file2)
