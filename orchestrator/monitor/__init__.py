from typing import Optional
from db import *
from .monitor import ProcessMonitor, ServerlessProcessMonitor
from .monetdb import MonetDBMonitor
from .sqlite import SQLiteMonitor


def get_monitor(db: str, directory: str) -> Optional[ProcessMonitor]:
    if db == DB_SQLITE:
        return SQLiteMonitor(directory)
    if db == DB_MONETDB:
        return MonetDBMonitor(directory)
    if db == DB_DUCKDB:
        return ServerlessProcessMonitor(directory, 'duckdb')
    if db in (AHEAD_CONT_REENC, AHEAD_CONT, AHEAD_EARLY, AHEAD_LATE, AHEAD_NORMAL):
        return ServerlessProcessMonitor(directory, 'ahead')
    return None
