#include "memory.hpp"
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <fcntl.h>

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
            char file[64];
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

        heap_stack* get_heap_and_stack_spaces(const int pid)
        {
            procmaps_iterator* maps = pmparser_parse(pid);

            if (maps == nullptr)
            {
                cout << "cannot parse the memory map of " << pid << endl;
                return nullptr;
            }

            //iterate over areas
            procmaps_struct* map;
            heap_stack* result = static_cast<heap_stack*>(malloc(sizeof(heap_stack)));

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
    }
}
