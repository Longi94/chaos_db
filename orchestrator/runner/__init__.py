from db import *
from .runner import SqlRunner
from .sqlite import SQLiteRunner


def get_runner(db):
    """
    :param db:
    :type db: str 
    :return: 
    :rtype: SqlRunner
    """
    if db == DB_SQLITE:
        return SQLiteRunner()
    return None
