from ctypes import cdll, c_char_p

lib = cdll.LoadLibrary('./build/debug/injector/libinject.so')
lib.inject(c_char_p(b"test"), c_char_p(b"test2"))
