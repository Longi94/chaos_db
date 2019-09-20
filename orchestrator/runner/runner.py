class SqlRunner(object):
    def __init__(self, directory):
        self.directory = directory
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
        :return: process id running the query or process id of the db service
        :rtype: int
        """
        raise NotImplementedError()

    def clean(self):
        """
        Clean up the database
        """
        raise NotImplementedError()
