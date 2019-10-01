from typing import Optional
from db import *
from .monitor import ProcessMonitor
from .sqlite import SQLiteMonitor


def get_monitor(db: str, directory: str, inject_delay: int) -> Optional[ProcessMonitor]:
    if db == DB_SQLITE:
        return SQLiteMonitor(directory, inject_delay)
    return None
