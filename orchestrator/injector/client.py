from socket import socket, AF_INET, SOCK_STREAM, SHUT_RDWR


class InjectorClient(object):

    def __init__(self, port: int, host: str = '127.0.0.1'):
        self.host = host
        self.port = port
        self.socket = None

    def connect(self):
        self.socket = socket(AF_INET, SOCK_STREAM)
        self.socket.connect((self.host, self.port))

    def send_start(self):
        self.socket.sendall(bytearray([1]))

    def send_stop(self):
        self.socket.sendall(bytearray([2]))

    def close(self):
        self.socket.shutdown(SHUT_RDWR)
        self.socket.close()
