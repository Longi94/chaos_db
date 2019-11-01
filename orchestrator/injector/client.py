import logging
import time
from socket import socket, AF_INET, SOCK_STREAM, SHUT_RDWR

log = logging.getLogger(__name__)


class InjectorClient(object):

    def __init__(self, port: int, host: str = '127.0.0.1'):
        self.host = host
        self.port = port
        self.socket = None

    def connect(self):
        self.socket = socket(AF_INET, SOCK_STREAM)
        attempt = 0
        while attempt < 12:
            try:
                self.socket.connect((self.host, self.port))
                break
            except ConnectionRefusedError as e:
                attempt += 1

                if attempt == 12:
                    log.error('Reached connection attempt limit.')
                    raise e
                else:
                    log.info('Connection failed. Retrying in 5 seconds...')
                    time.sleep(5)

    def send_start(self):
        self.socket.sendall(bytearray([1]))

    def send_stop(self):
        self.socket.sendall(bytearray([2]))

    def close(self):
        self.socket.shutdown(SHUT_RDWR)
        self.socket.close()
