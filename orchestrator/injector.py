import subprocess
import logging
import os
from typing import List, Any

log = logging.getLogger(__name__)

INJECTOR_PATH = './build/release/injector/injector'


def check_injector():
    """
    Stops execution if the injector was not found.
    """
    if not os.path.exists(INJECTOR_PATH):
        log.error('injector not found')
        exit(1)


def run_injector(output_file: str, input_file: str, inject_delay: int, child_command: List[Any]) -> subprocess.Popen:
    command = [INJECTOR_PATH, '-o', output_file]

    if input_file is not None:
        command.extend(['-i', input_file])

    if inject_delay is not None:
        command.extend(['-m', str(inject_delay)])

    command.append('-c')
    command.extend(child_command)

    log.info('Running command: ' + ' '.join(command))
    p = subprocess.Popen(command, stdout=subprocess.PIPE)

    return p
