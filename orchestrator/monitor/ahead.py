import os
import subprocess
from result import *
from queries import TPCH_UPDATES
from .monitor import ServerlessProcessMonitor


class AheadMonitor(ServerlessProcessMonitor):

    def __init__(self, directory: str, database_dir: str):
        super().__init__(directory, database_dir, 'ahead')

    def evaluate_result(self):
        super().evaluate_result()

        with open(os.path.join(self.directory, 'stderr.txt'), 'r') as stderr:
            stderr_lines = stderr.readlines()

        self.detected = any(filter(lambda x: 'bitflip detected' in x, stderr_lines))
