import sqlite3
from typing import Set

DB_SQLITE = 'sqlite'
DB_MONETDB = 'monetdb'


class ResultsDatabase(object):

    def __init__(self, file):
        self.connection = sqlite3.connect(file)
        cursor = self.connection.cursor()
        cursor.execute(
            '''CREATE TABLE IF NOT EXISTS result (
                   iteration      INTEGER NOT NULL,
                   hostname       TEXT    NOT NULL,
                   result         INTEGER NOT NULL,
                   exited         INTEGER NOT NULL,
                   return_code    INTEGER NOT NULL,
                   signaled       INTEGER NOT NULL,
                   term_sig       INTEGER NOT NULL,
                   runtime        REAL    NOT NULL,
                   fault_count    INTEGER NOT NULL,
                   max_heap_size  INTEGER NOT NULL,
                   max_stack_size INTEGER NOT NULL
               )'''
        )
        self.connection.commit()
        cursor.close()

    def insert_result(self, result):
        cursor = self.connection.cursor()
        cursor.execute('''INSERT INTO result VALUES 
                          (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)''', result)
        self.connection.commit()
        cursor.close()

    def get_iterations(self) -> Set[int]:
        cursor = self.connection.cursor()
        cursor.execute('''SELECT iteration FROM result''')
        iterations = set(map(lambda row: row[0], cursor.fetchall()))
        cursor.close()
        return iterations

    def close(self):
        self.connection.close()
