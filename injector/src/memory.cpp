#include "memory.hpp"
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <memory>
#include <fcntl.h>
#include <random>

extern "C" {
#include "pmparser.h"
}

using namespace std;

namespace chaos
{
    namespace memory
    {
        int open_mem(const int pid)
        {
            char file[16];
            sprintf(file, "/proc/%d/mem", pid);
            return open(file, O_RDWR);
        }

        int read_byte(const int fd, void* buf, const off_t address)
        {
            const ssize_t bytes_read = pread(fd, buf, 1, address);

            if (bytes_read < 0)
            {
                cout << "Error while reading bytes: " << strerror(errno) << endl;
                return -1;
            }
            if (bytes_read == 0)
            {
                cout << "Read 0 bytes" << endl;
                return -1;
            }

            return 0;
        }

        int write_byte(const int fd, void* buf, const off_t address)
        {
            const ssize_t bytes_written = pwrite(fd, buf, 1, address);

            if (bytes_written < 0)
            {
                cout << "Error while writing bytes: " << strerror(errno) << endl;
                return -1;
            }
            if (bytes_written == 0)
            {
                cout << "Read 0 bytes" << endl;
                return -1;
            }

            return 0;
        }

        unique_ptr<heap_stack> get_heap_and_stack_spaces(const int pid)
        {
            procmaps_iterator* maps = pmparser_parse(pid);

            if (maps == nullptr)
            {
                cout << "cannot parse the memory map of " << pid << endl;
                return nullptr;
            }

            //iterate over areas
            procmaps_struct* map;
            unique_ptr<heap_stack> result(new heap_stack());

            while ((map = pmparser_next(maps)) != nullptr)
            {
                if (strcmp(map->pathname, "[heap]") == 0)
                {
                    result->heap_start = reinterpret_cast<off_t>(map->addr_start);
                    result->heap_end = reinterpret_cast<off_t>(map->addr_end);
                } else if (strcmp(map->pathname, "[stack]") == 0)
                {
                    result->stack_start = reinterpret_cast<off_t>(map->addr_start);
                    result->stack_end = reinterpret_cast<off_t>(map->addr_end);
                }
            }

            pmparser_free(maps);

            return result;
        }

        off_t get_random_address(const pid_t pid, const space m_space, mt19937 &rng) {
            const auto memory_info = get_heap_and_stack_spaces(pid);

            off_t addr = 0;

            if (memory_info != nullptr)
            {
                cout << "Heap: " << hex << memory_info->heap_start << "-" << memory_info->heap_end << " " <<
                    dec << memory_info->heap_end - memory_info->heap_start << endl;

                cout << "Stack: " << hex << memory_info->stack_start << "-" << memory_info->stack_end << " " <<
                    dec << memory_info->stack_end - memory_info->stack_start << endl;

                const auto heap_size = memory_info->heap_end - memory_info->heap_start;
                const auto stack_size = memory_info->stack_end - memory_info->stack_start;

                switch (m_space) {
                    case heap:
                        {
                            cout << "Choosing address from heap." << endl;
                            uniform_int_distribution<off_t> address_dist(memory_info->heap_start, memory_info->heap_end);
                            addr = address_dist(rng);
                            break;
                        }
                    case stack:
                        {
                            cout << "Choosing address from stack." << endl;
                            uniform_int_distribution<off_t> address_dist(memory_info->stack_start, memory_info->stack_end);
                            addr = address_dist(rng);
                            break;
                        }
                    default:
                        {
                            cout << "Choosing address from stack or heap." << endl;
                            // Randomly choose an address in heap or stack
                            uniform_int_distribution<off_t> address_dist(0, heap_size + stack_size);
                            const off_t rand_i = address_dist(rng);

                            if (rand_i > heap_size)
                            {
                                addr = memory_info->stack_start - heap_size + rand_i;
                            }
                            else
                            {
                                addr = memory_info->heap_start + rand_i;
                            }
                            break;
                        }
                }
            }

            return addr;
        }
    }
}
