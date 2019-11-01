import argparse


class SqlRunner(object):

    def __init__(self, directory: str, args: argparse.Namespace):
        self.directory = directory
        self.fault = args.fault
        self.inject_space = args.inject_space
        self.flip_rate = args.flip_rate
        self.random_flip_rate = args.random_flip_rate
        self.mean_runtime = args.mean_runtime
        self.single = args.single
        self.database_dir = args.database_path
        self.serverless = False
        self.server_process = None
        self.query_process = None

    def init_db(self):
        """
        Initialize the TPC-H database.
        """
        raise NotImplementedError()

    def start_server(self):
        """
        Start the database server.
        """
        pass

    def run_query(self, query: int):
        """
        Run a TPC-H query.

        :param query: number of the query (1-22)
        """
        raise NotImplementedError()

    def finish_query(self):
        pass

    def clean(self):
        """
        Clean up the database
        """
        raise NotImplementedError()
