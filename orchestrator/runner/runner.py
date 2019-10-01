class SqlRunner(object):
    def __init__(self, directory: str, inject_delay: int):
        self.directory = directory
        self.inject_delay = inject_delay
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
