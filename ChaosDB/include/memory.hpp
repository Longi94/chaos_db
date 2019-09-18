#pragma once
#include <unistd.h>

namespace chaos
{
    namespace memory
    {
        int detach_from_process(int pid);
        int attach_to_process(int pid);
        int open_mem(int pid);
        void read_byte(int fd, void* buf, off_t address);
        void write_byte(int fd, void* buf, off_t address);
    }
}

