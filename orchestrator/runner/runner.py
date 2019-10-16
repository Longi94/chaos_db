import argparse


class SqlRunner(object):
    def __init__(self, directory: str, inject_delay: int, args: argparse.Namespace):
        self.directory = directory
        self.inject_delay = inject_delay
        self.fault = args.fault
        self.inject_space = args.inject_space
        self.flip_rate = args.flip_rate
        self.random_flip_rate = args.random_flip_rate
        self.process = None

    def init_db(self):
        """
        Initialize the TPC-H database.
        """
        raise NotImplementedError()

    def run_tpch(self, query: int):
        """
        Run a TPC-H query.

        :param query: number of the query (1-22)
        """
        raise NotImplementedError()

    def clean(self):
        """
        Clean up the database
        """
        raise NotImplementedError()
