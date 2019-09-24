from time import time


class ProcessMonitor(object):
    def __init__(self, directory, inject_delay):
        self.directory = directory
        self.query = None
        self.start_time = None
        self.end_time = None
        self.inject_delay = inject_delay
        self.return_code = None

    def start(self, query):
        """
        :type query: int
        """
        self.query = query
        self.start_time = time()

    def end(self):
        self.end_time = time()
        self.evaluate_result()

    def evaluate_result(self):
        raise NotImplementedError()

    def print_result(self):
        print('Process return code: {}'.format(self.return_code))
        print('Process run time: {} seconds'.format(self.end_time - self.start_time))

    def to_dict(self):
        return {
            'runtime': self.end_time - self.start_time,
            'return_code': self.return_code,
            'inject_delay': self.inject_delay
        }
