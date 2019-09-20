import pandas as pd
from .monitor import ProcessMonitor


class SQLiteMonitor(ProcessMonitor):
    def evaluate_result(self):
        if self.return_code == 0:

            output = pd.read_csv('result/sqlite/q{}.out'.format(self.query), sep='|')
            expected = pd.read_csv('answers/q{}.out'.format(self.query), sep='|')
            pass
