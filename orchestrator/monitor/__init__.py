from db import *
from .monitor import ProcessMonitor
from .sqlite import SQLiteMonitor


def get_monitor(db):
    """
    :param db:
    :type db: str 
    :return: 
    :rtype: ProcessMonitor
    """
    if db == DB_SQLITE:
        return SQLiteMonitor()
    return None
