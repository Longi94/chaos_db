import os
import platform


def get_hostname() -> str:
    return os.getenv('HOSTNAME', os.getenv('COMPUTERNAME', platform.node())).split('.')[0]
