import subprocess
import logging

log = logging.getLogger(__name__)


def inject_bit_flip(pid):
    """
    :type pid: int
    """
    command = ['./build/ChaosDB/injector', str(pid)]
    log.info('Running command: ' + ' '.join(command))
    p = subprocess.Popen(command, stdout=subprocess.PIPE)
    for line in iter(p.stdout.readline, b''):
        log.info(line.decode("utf-8").strip())
