from typing import Optional
from db import *
from .monitor import ProcessMonitor, ServerlessProcessMonitor
from .monetdb import MonetDBMonitor
from .sqlite import SQLiteMonitor
from .ahead import AheadMonitor


def get_monitor(db: str, database_dir: str, iteration: int, directory: str) -> Optional[ProcessMonitor]:
    if db == DB_SQLITE:
        return SQLiteMonitor(iteration, directory, database_dir)
    if db == DB_MONETDB:
        return MonetDBMonitor(iteration, directory, database_dir)
    if db == DB_DUCKDB:
        return ServerlessProcessMonitor(iteration, directory, database_dir, 'duckdb')
    if db in (AHEAD_CONT_REENC, AHEAD_CONT, AHEAD_EARLY, AHEAD_LATE, AHEAD_NORMAL):
        return AheadMonitor(iteration, directory, database_dir)
    return None
