from typing import Optional
from db import *
from .monitor import ProcessMonitor
from .sqlite import SQLiteMonitor
from .monetdb import MonetDBMonitor
from .duckdb import DuckDBMonitor


def get_monitor(db: str, directory: str) -> Optional[ProcessMonitor]:
    if db == DB_SQLITE:
        return SQLiteMonitor(directory)
    if db == DB_MONETDB:
        return MonetDBMonitor(directory)
    if db == DB_DUCKDB:
        return DuckDBMonitor(directory)
    return None
