import os
import logging
import argparse
from distutils.dir_util import copy_tree
from shutil import rmtree
from injector import run_injector
from .runner import SqlRunner
from queries import *
from db import *

log = logging.getLogger(__name__)


class AheadRunner(SqlRunner):
    def __init__(self, directory: str, db_type: str, args: argparse.Namespace):
        super(AheadRunner, self).__init__(directory, args)
        self.db_type = db_type
        self.db_path = os.path.join(directory, 'data')
        self.serverless = True

    def init_db(self):
        log.info('Copying ahead database to a temp folder...')
        log.info('Temp file name: ' + self.db_path)
        copy_tree(os.path.join(self.database_dir, 'data'), self.db_path)

    def run_query(self, query: int):
        binary_file = 'chaos-ssbm'

        if query == SSB11:
            binary_file += '-q11'
        elif query == TPCH3:
            binary_file += '-q41'
        else:
            raise ValueError(f'Unsupported query: {query}')

        if self.db_type == AHEAD_NORMAL:
            binary_file += '_normal'
        elif self.db_type == AHEAD_LATE:
            binary_file += '_late'
        elif self.db_type == AHEAD_EARLY:
            binary_file += '_early'
        elif self.db_type == AHEAD_CONT:
            binary_file += '_continuous'
        elif self.db_type == AHEAD_CONT_REENC:
            binary_file += '_continuous_reenc'
        else:
            raise ValueError(f'Unsupported db type: {self.db_type}')

        binary_file = os.path.join(self.database_dir, 'build/Release', binary_file)

        with open(os.path.join(self.directory, 'inject_stderr.txt'), 'w') as f:
            self.query_process = run_injector(
                output_file=os.path.join(self.directory, 'output.txt'),
                error_file=os.path.join(self.directory, 'stderr.txt'),
                child_command=[binary_file, '-d', self.db_path],
                fault=self.fault,
                inject_space=self.inject_space,
                flip_rate=self.flip_rate,
                random_flip_rate=self.random_flip_rate,
                mean_runtime=self.mean_runtime,
                inject_stderr=f,
                single=self.single
            )

    def clean(self):
        rmtree(self.db_path)
