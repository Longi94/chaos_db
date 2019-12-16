import subprocess
import logging
import os
import sys
from queue import Queue
from typing import List, Any, Optional

log = logging.getLogger(__name__)

INJECTOR_PATH = './build/release/injector/injector'

_port_pool: Optional[Queue] = None


def init_pool(size: int):
    global _port_pool
    _port_pool = Queue(size * 2)
    for i in range(7000, 7000 + size * 2):
        _port_pool.put(i)


def reserve_port() -> int:
    global _port_pool

    if _port_pool is None:
        raise RuntimeError('port pool not initialized')

    return _port_pool.get()


def release_port(port: int):
    global _port_pool

    if _port_pool is None:
        raise RuntimeError('port pool not initialized')

    _port_pool.put(port)


def check_injector():
    """
    Stops execution if the injector was not found.
    """
    if not os.path.exists(INJECTOR_PATH):
        log.error('injector not found')
        exit(1)


def run_injector(
        child_command: List[Any],
        database: str,
        iteration: int,
        hostname: str,
        input_file: Optional[str] = None,
        fault: Optional[str] = None,
        inject_space: Optional[str] = None,
        flip_rate: Optional[float] = None,
        random_flip_rate: bool = False,
        mean_runtime: Optional[float] = None,
        single: bool = False,
        port: Optional[int] = None
) -> subprocess.Popen:
    command = [INJECTOR_PATH, '-d', database, '-a', str(iteration), '-b', hostname]

    if input_file is not None:
        command.extend(['-i', input_file])

    if fault is not None:
        command.extend(['-f', fault])

    if inject_space is not None:
        command.extend(['-s', inject_space])

    if flip_rate is not None:
        command.extend(['--flip-rate', str(flip_rate)])

    if mean_runtime is not None:
        command.extend(['--mean-runtime', str(mean_runtime)])

    if random_flip_rate:
        command.append('--random-flip-rate')

    if single:
        command.append('--single')

    if port is not None:
        command.extend(['-p', str(port)])

    command.append('-c')
    command.extend(child_command)

    log.info('Running command: ' + ' '.join(command))
    p = subprocess.Popen(command, stdout=sys.stdout.buffer, stderr=sys.stderr.buffer, preexec_fn=os.setpgrp)

    return p
