class SqlRunner(object):
    def __init__(self, directory, inject_delay):
        self.directory = directory
        self.inject_delay = inject_delay
        self.process = None

    def init_db(self):
        """
        Initialize the TPC-H database.
        """
        raise NotImplementedError()

    def run_tpch(self, query):
        """
        Run a TPC-H query.

        :param query: number of the query (1-22)
        :type query: int
        """
        raise NotImplementedError()

    def clean(self):
        """
        Clean up the database
        """
        raise NotImplementedError()
