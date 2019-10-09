#pragma once
#include <memory>
#include <random>

namespace chaos
{
    namespace memory
    {
        struct heap_stack
        {
            off_t heap_start = 0;
            off_t heap_end = 0;
            off_t stack_start = 0;
            off_t stack_end = 0;
            long heap_size = 0;
            long stack_size = 0;
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
        std::unique_ptr<heap_stack> get_heap_and_stack_spaces(int pid);

        /**
         * Get a random virtual address from the memory space of a process.
         * @param memory_info memory info of the child process
         * @param m_space the part of the memory to get the address from
         * @param rng random number generator
         */
        off_t get_random_address(const std::unique_ptr<heap_stack>& memory_info, space m_space, std::mt19937& rng);
    }
}
