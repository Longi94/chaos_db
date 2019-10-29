from threading import RLock


class LockProxy(object):
    def __init__(self, obj):
        self.__obj = obj
        # RLock because object methods may call own methods
        self.__lock = RLock()

    def __getattr__(self, name):
        def wrapped(*a, **k):
            with self.__lock:
                getattr(self.__obj, name)(*a, **k)

        return wrapped
