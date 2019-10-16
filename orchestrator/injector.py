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


def run_injector(output_file: str, input_file: Optional[str], error_file: Optional[str], inject_delay: Optional[int],
                 child_command: List[Any], fault: Optional[str], inject_space: Optional[str],
                 flip_rate: float, random_flip_rate: bool) -> subprocess.Popen:
    command = [INJECTOR_PATH, '-o', output_file]

    if input_file is not None:
        command.extend(['-i', input_file])

    if input_file is not None:
        command.extend(['-e', error_file])

    if inject_delay is not None:
        command.extend(['-d', str(inject_delay)])

    if fault is not None:
        command.extend(['-f', fault])

    if inject_space is not None:
        command.extend(['-s', inject_space])

    if flip_rate is not None:
        command.extend(['--flip-rate', str(flip_rate)])

    if random_flip_rate:
        command.append('--random-flip-rate')

    command.append('-c')
    command.extend(child_command)

    log.info('Running command: ' + ' '.join(command))
    p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    return p
