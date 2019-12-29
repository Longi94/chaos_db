from .monitor import ServerlessProcessMonitor
from db import Result


class AheadMonitor(ServerlessProcessMonitor):

    def __init__(self, iteration: int, directory: str, database_dir: str):
        super().__init__(iteration, directory, database_dir, 'ahead')

    def evaluate_result(self, result: Result):
        super().evaluate_result(result)
