import os
import platform
import subprocess
from fault_type import FAULT_BIT_FLIP


def get_hostname() -> str:
    return os.getenv('HOSTNAME', os.getenv('COMPUTERNAME', platform.node())).split('.')[0]


def get_dir_name(database: str, query: str, fault: str, inject_space: str, flip_rate: float,
                 random_flip_rate: bool) -> str:
    name = f'{database}_{query}'

    if fault is not None:
        name += f'_{fault}'

    if inject_space is not None:
        name += f'_{inject_space}'

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
