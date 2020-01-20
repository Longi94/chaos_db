import argparse
from db import *
from .runner import SqlRunner
from .sqlite import SQLiteRunner
from .monetdb import MonetDBRunner
from .duckdb import DuckDBRunner
from .ahead import AheadRunner


def get_runner(db, iteration: int, directory: str, args: argparse.Namespace, hostname: str, results_db: str) \
        -> Optional[SqlRunner]:
    if db == DB_SQLITE:
        return SQLiteRunner(directory, args, iteration, hostname, results_db)
    if db == DB_MONETDB:
        return MonetDBRunner(directory, args, iteration, hostname, results_db)
    if db == DB_DUCKDB:
        return DuckDBRunner(directory, args, iteration, hostname, results_db)
    if db in (AHEAD_CONT_REENC, AHEAD_CONT, AHEAD_EARLY, AHEAD_LATE, AHEAD_NORMAL):
        return AheadRunner(directory, db, args, iteration, hostname, results_db)
    return None
