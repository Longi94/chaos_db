from typing import Optional
from db import *
from .monitor import ProcessMonitor
from .sqlite import SQLiteMonitor
from .monetdb import MonetDBMonitor


def get_monitor(db: str, directory: str) -> Optional[ProcessMonitor]:
    if db == DB_SQLITE:
        return SQLiteMonitor(directory)
    if db == DB_MONETDB:
        return MonetDBMonitor(directory)
    return None
