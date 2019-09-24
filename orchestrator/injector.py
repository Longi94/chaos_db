import subprocess
import logging

log = logging.getLogger(__name__)


def run_injector(output_file, input_file, inject_delay, child_command):
    command = ['./build/release/ChaosDB/injector', '-o', output_file]

    if input_file is not None:
        command.extend(['-i', input_file])

    if inject_delay is not None:
        command.extend(['-m', str(inject_delay)])

    command.append('-c')
    command.extend(child_command)

    log.info('Running command: ' + ' '.join(command))
    p = subprocess.Popen(command, stdout=subprocess.PIPE)
    for line in iter(p.stdout.readline, b''):
        log.info(line.decode("utf-8").strip())

    return p
