from db import *
from .runner import SqlRunner
from .sqlite import SQLiteRunner


def get_runner(db, dir, inject_delay):
    """
    :param db:
    :type db: str 
    :return: 
    :rtype: SqlRunner
    """
    if db == DB_SQLITE:
        return SQLiteRunner(dir, inject_delay)
    return None
