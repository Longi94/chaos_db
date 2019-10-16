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

    def monitor(self, process: Popen):
        for line in iter(process.stdout.readline, b''):
            line = line.decode("utf-8").strip()
            log.info(line)

            if line.startswith('WIFEXITED: '):
                self.exited = line[11:] == '1'
            if line.startswith('WEXITSTATUS: '):
                self.return_code = int(line[13:])
            if line.startswith('WIFSIGNALED: '):
                self.signaled = line[13:] == '1'
            if line.startswith('WTERMSIG: '):
                self.term_sig = int(line[10:])

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
            'result': self.result,
            'exited': self.exited,
            'signaled': self.signaled,
            'term_sig': self.term_sig,
        }

    def compare_files(self, file1: str, file2: str) -> bool:
        return filecmp.cmp(file1, file2)
