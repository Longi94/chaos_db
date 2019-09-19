#pragma once
#include <unistd.h>

namespace chaos
{
    namespace memory
    {
        struct heap_stack
        {
            off_t heap_start;
            off_t heap_end;
            off_t stack_start;
            off_t stack_end;
        };

        int detach_from_process(int pid);
        int attach_to_process(int pid);
        int open_mem(int pid);
        int read_byte(int fd, void* buf, off_t address);
        int write_byte(int fd, void* buf, off_t address);
        heap_stack* get_heap_and_stack_spaces(int pid);
    }
}

