from typing import Optional
from db import *
from .runner import SqlRunner
from .sqlite import SQLiteRunner


def get_runner(db, directory: str, inject_delay: int) -> Optional[SqlRunner]:
    if db == DB_SQLITE:
        return SQLiteRunner(directory, inject_delay)
    return None
