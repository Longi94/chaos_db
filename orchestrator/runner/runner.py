import argparse


class SqlRunner(object):

    def __init__(self, directory: str, args: argparse.Namespace, iteration: int, hostname: str, results_db: str):
        self.results_db = results_db
        self.hostname = hostname
        self.iteration = iteration
        self.directory = directory
        self.fault = args.fault
        self.inject_to_heap = args.heap
        self.inject_to_stack = args.stack
        self.inject_to_anon = args.anon
        self.flip_rate = args.flip_rate
        self.random_flip_rate = args.random_flip_rate
        self.mean_runtime = args.mean_runtime
        self.single = args.single
        self.database_dir = args.database_path
        self.serverless = False
        self.server_process = None
        self.query_process = None
        self.debug = args.debug

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
