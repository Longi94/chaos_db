import os
import logging
import argparse
from queue import Queue
from distutils.dir_util import copy_tree
from shutil import rmtree
from injector import run_injector, reserve_port, release_port
from .runner import SqlRunner
from queries import *

log = logging.getLogger(__name__)

mserver5_port_pool = None


def init_mserver5_port_pool(size: int):
    global mserver5_port_pool
    mserver5_port_pool = Queue(size)
    for i in range(50001, 50001 + size):
        mserver5_port_pool.put(i)


class MonetDBRunner(SqlRunner):

    def __init__(self, iteration: int, directory: str, args: argparse.Namespace):
        super().__init__(directory, args)
        self.db_name = f'data_{iteration}'
        self.db_path = os.path.join(directory, self.db_name)
        self.injector_port = None
        self.server_port = None

    def init_db(self):
        log.info('Copying monetdb sqlite database to a temp folder...')
        log.info('Temp folder name: ' + self.db_path)
        copy_tree(os.path.join(self.database_dir, 'data'), self.db_path)

        global mserver5_port_pool
        self.injector_port = reserve_port()
        self.server_port = mserver5_port_pool.get()

    def run_query(self, query: int):
        query_file = None

        if query == TPCH1:
            query_file = 'databases/sqlite/queries/1.sql'
        elif query == TPCH3:
            query_file = 'databases/sqlite/queries/3.sql'

        if query_file is None:
            raise NameError(f'Unknown query: {query}')

        with open(os.path.join(self.directory, 'inject_stderr.txt'), 'w') as f:
            self.process = run_injector(
                output_file=os.path.join(self.directory, 'output.txt'),
                input_file=query_file,
                error_file=os.path.join(self.directory, 'stderr.txt'),
                child_command=[
                    os.path.join(self.database_dir, 'build/bin/mserver5'),
                    f'--dbpath={self.db_path}',
                    '--daemon=yes',
                    '--set',
                    'gdk_mmap_minsize=1000000000000',
                    '--set',
                    f'mapi_port={self.server_port}'
                ],
                fault=self.fault,
                inject_space=self.inject_space,
                flip_rate=self.flip_rate,
                random_flip_rate=self.random_flip_rate,
                mean_runtime=self.mean_runtime,
                inject_stderr=f,
                single=self.single,
                port=self.injector_port
            )

    def clean(self):
        global mserver5_port_poo
        release_port(self.injector_port)
        mserver5_port_pool.put(self.server_port)
        rmtree(self.db_path)
