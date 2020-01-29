#pragma once
#include <memory>
#include <random>

namespace chaos
{
    namespace memory
    {
        struct memory_map
        {
            off_t start = 0;
            off_t end = 0;
            long size = 0;
        };

        struct heap_stack
        {
            std::unique_ptr<memory_map> heap = nullptr;
            std::unique_ptr<memory_map> stack = nullptr;
            std::unique_ptr<std::vector<std::unique_ptr<memory_map>>> anons = nullptr;
        };

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
         * Check if the address in the memory info.
         * @param address virtual memory address
         * @param memory_info memory info of the child process
         * @return true if the address is in the memory space
         */
        inline bool is_in_memory(off_t address, const std::unique_ptr<heap_stack>& memory_info);
    }
}
