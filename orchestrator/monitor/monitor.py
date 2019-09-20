from time import time
from subprocess import Popen


class ProcessMonitor(object):
    def __init__(self, directory):
        self.directory = directory
        self.process = None
        self.query = None
        self.start_time = None
        self.end_time = None
        self.inject_time = None
        self.return_code = None

    def start(self, process, query):
        """
        :type process: Popen
        :type query: int
        """
        self.process = process
        self.query = query
        self.start_time = time()

    def set_inject_time(self, inject_time=None):
        """
        :type inject_time: float
        """
        if inject_time is None:
            self.inject_time = time()
        else:
            self.inject_time = inject_time

    def end(self):
        self.end_time = time()
        self.return_code = self.process.returncode
        self.evaluate_result()

    def evaluate_result(self):
        raise NotImplementedError()

    def print_result(self):
        print('Process return code: {}'.format(self.return_code))
        print('Process run time: {} seconds'.format(self.end_time - self.start_time))
