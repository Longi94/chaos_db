import os
import time
import logging
import argparse
import subprocess
from queue import Queue
from distutils.dir_util import copy_tree
from shutil import rmtree
from injector import run_injector, reserve_port, release_port
from injector.client import InjectorClient
from .runner import SqlRunner
from queries import *

log = logging.getLogger(__name__)

mserver5_port_pool = None


def init_mserver5_port_pool(size: int):
    global mserver5_port_pool
    mserver5_port_pool = Queue(size * 2)
    for i in range(50001, 50001 + size * 2):
        mserver5_port_pool.put(i)


class MonetDBRunner(SqlRunner):

    def __init__(self, directory: str, args: argparse.Namespace, iteration: int, hostname: str, results_db: str):
        super().__init__(directory, args, iteration, hostname, results_db)
        self.db_name = f'data_{iteration}'
        self.db_path = os.path.join(directory, self.db_name)
        self.injector_port = None
        self.server_port = None
        self.injector_client = None

    def init_db(self):
        log.info('Copying monetdb sqlite database to a temp folder...')
        log.info('Temp folder name: ' + self.db_path)
        copy_tree(os.path.join(self.database_dir, 'data'), self.db_path)

        global mserver5_port_pool
        self.injector_port = reserve_port()
        self.server_port = mserver5_port_pool.get()
        self.injector_client = InjectorClient(self.injector_port)

    def start_server(self):
        self.server_process = run_injector(
            database=self.results_db,
            iteration=self.iteration,
            hostname=self.hostname,
            child_command=[
                os.path.join(self.database_dir, 'build/bin/mserver5'),
                f'--dbpath={self.db_path}',
                '--daemon=yes',
                '--set',
                'gdk_mmap_minsize=1000000000000',  # force monetdb to use malloc instead of mmap
                '--set',
                'gdk_mmap_minsize_persistent=1000000000000',
                '--set',
                'gdk_mmap_minsize_transient=1000000000000',
                '--set',
                'sql_optimizer=sequential_pipe',  # disable parallelism to avoid non deterministic output
                '--set',
                f'mapi_port={self.server_port}'
            ],
            fault=self.fault,
            inject_to_heap=self.inject_to_heap,
            inject_to_stack=self.inject_to_stack,
            inject_to_anon=self.inject_to_anon,
            flip_rate=self.flip_rate,
            random_flip_rate=self.random_flip_rate,
            mean_runtime=self.mean_runtime,
            single=self.single,
            port=self.injector_port
        )

        test_query = [
            os.path.join(self.database_dir, 'build/bin/mclient'),
            '--port',
            str(self.server_port),
            '-s',
            'SELECT 1',
            self.db_name
        ]

        # a bit of time for the tcp socket to initialize
        time.sleep(0.2)

        wait_attempt = 0

        while True:
            p = subprocess.Popen(test_query, stderr=subprocess.DEVNULL, stdout=subprocess.DEVNULL)
            p.wait()
            if p.returncode == 0:
                break
            else:
                if wait_attempt >= 60:
                    # 1 minute timeout
                    raise TimeoutError('mserver5 start timeout')
                log.info('Waiting for mserver5...')
                wait_attempt += 1
                time.sleep(1)

    def run_query(self, query: int):
        query_file = None

        if query == TPCH1:
            query_file = 'databases/monetdb/queries/1.sql'
        elif query == TPCH3:
            query_file = 'databases/monetdb/queries/3.sql'

        if query_file is None:
            raise NameError(f'Unknown query: {query}')

        self.injector_client.connect()

        query_command = [
            os.path.join(self.database_dir, 'build/bin/mclient'),
            f'--port={self.server_port}',
            '--format=csv+|',
            self.db_name,
            query_file
        ]

        log.info(f'Running query: {" ".join(query_command)}')

        self.query_process = subprocess.Popen(query_command, preexec_fn=os.setpgrp, stdout=subprocess.PIPE,
                                              stderr=subprocess.PIPE)
        self.injector_client.send_start()

    def finish_query(self):
        self.injector_client.send_stop()
        self.injector_client.close()

    def clean(self):
        global mserver5_port_pool
        release_port(self.injector_port)
        mserver5_port_pool.put(self.server_port)
        rmtree(self.db_path)
