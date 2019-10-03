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

        enum space { all, heap, stack };

        /**
         * Open the memory file of a process. /proc/<pid>/mem
         * @param pid process id
         * @return new file descriptor
         */
        int open_mem(int pid);

        /**
         * Read a byte from memory.
         * @param fd file descriptor used for reading
         * @param buf buffer the read byte will be written into
         * @param address the address of the byte
         * @return 0 if successful, -1 if there was an error
         */
        int read_byte(int fd, void* buf, off_t address);

        /**
         * Write a byte to memory.
         * @param fd file descriptor used for writing
         * @param buf the first byte of the buffer will be written to memory
         * @param address the address of the byte
         * @return 0 if successful, -1 if there was an error
         */
        int write_byte(int fd, void* buf, off_t address);

        /**
         * Parses the memory map file using https://github.com/ouadev/proc_maps_parser. Finds the address space fo the heap and stack.
         * @param pid the process id
         * @return pointer to the heap_stack struct
         */
        heap_stack* get_heap_and_stack_spaces(int pid);

        /**
         * Get a random virtual address from the memory space of a process.
         * @param pid the id of the process
         * @param m_space the part of the memory to get the address from
         */
        off_t get_random_address(pid_t pid, space m_space);
    }
}

