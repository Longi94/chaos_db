import subprocess
import logging
import os
from typing import List, Any, Optional

log = logging.getLogger(__name__)

INJECTOR_PATH = './build/release/injector/injector'


def check_injector():
    """
    Stops execution if the injector was not found.
    """
    if not os.path.exists(INJECTOR_PATH):
        log.error('injector not found')
        exit(1)


def run_injector(output_file: str, input_file: Optional[str], error_file: Optional[str],
                 child_command: List[Any], fault: Optional[str], inject_space: Optional[str],
                 flip_rate: float, random_flip_rate: bool, mean_runtime: float, inject_stderr,
                 single: bool) -> subprocess.Popen:
    command = [INJECTOR_PATH, '-o', output_file]

    if input_file is not None:
        command.extend(['-i', input_file])

    if input_file is not None:
        command.extend(['-e', error_file])

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

    command.append('-c')
    command.extend(child_command)

    log.info('Running command: ' + ' '.join(command))
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=inject_stderr)

    return p
