from db import *
from .runner import SqlRunner
from .sqlite import SQLiteRunner


def get_runner(db, dir):
    """
    :param db:
    :type db: str 
    :return: 
    :rtype: SqlRunner
    """
    if db == DB_SQLITE:
        return SQLiteRunner(dir)
    return None
