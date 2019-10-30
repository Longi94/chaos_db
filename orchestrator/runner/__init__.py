import argparse
from typing import Optional
from db import *
from .runner import SqlRunner
from .sqlite import SQLiteRunner
from .monetdb import MonetDBRunner
from .duckdb import DuckDBRunner


def get_runner(db, iteration: int, directory: str, args: argparse.Namespace) -> Optional[SqlRunner]:
    if db == DB_SQLITE:
        return SQLiteRunner(directory, args)
    if db == DB_MONETDB:
        return MonetDBRunner(iteration, directory, args)
    if db == DB_DUCKDB:
        return DuckDBRunner(directory, args)
    return None
