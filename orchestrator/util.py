import os
import platform
import subprocess
from fault_type import FAULT_BIT_FLIP


def get_hostname() -> str:
    return os.getenv('HOSTNAME', os.getenv('COMPUTERNAME', platform.node())).split('.')[0]


def get_dir_name(
        database: str,
        query: str,
        fault: str,
        flip_rate: float,
        random_flip_rate: bool,
        inject_to_heap: bool = False,
        inject_to_anon: bool = False,
        inject_to_stack: bool = False
) -> str:
    name = f'{database}_{query}'

    if fault is not None:
        name += f'_{fault}'

    if inject_to_heap:
        name += '_x'

    if inject_to_anon:
        name += '_y'

    if inject_to_stack:
        name += '_z'

    if fault == FAULT_BIT_FLIP:
        name += '_{0:g}'.format(flip_rate)

        if random_flip_rate:
            name += '_randomized'

    return name


def kill_family(pid: int, signal: int):
    try:
        pid_str = subprocess.check_output(['pgrep', '-P', str(pid)]).decode('utf-8').strip()
    except subprocess.CalledProcessError:
        try:
            os.kill(pid, signal)
        except ProcessLookupError:
            pass
        return

    if len(pid_str) == 0:
        return

    children = list(map(int, pid_str.split('\n')))

    for child in children:
        kill_family(child, signal)

    try:
        os.kill(pid, signal)
    except ProcessLookupError:
        pass
