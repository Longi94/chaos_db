from db import *
from .monitor import ProcessMonitor
from .sqlite import SQLiteMonitor


def get_monitor(db, directory, inject_delay):
    """
    :type db: str
    :type directory: str
    :rtype: ProcessMonitor
    """
    if db == DB_SQLITE:
        return SQLiteMonitor(directory, inject_delay)
    return None
