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
                cerr << "Error while reading bytes: " << strerror(errno) << endl;
                return -1;
            }
            if (bytes_read == 0)
            {
                cerr << "Read 0 bytes" << endl;
                return -1;
            }

            return 0;
        }

        int write_byte(const int fd, void* buf, const off_t address)
        {
            const ssize_t bytes_written = pwrite(fd, buf, 1, address);

            if (bytes_written < 0)
            {
                cerr << "Error while writing bytes: " << strerror(errno) << endl;
                return -1;
            }
            if (bytes_written == 0)
            {
                cerr << "Written 0 bytes" << endl;
                return -1;
            }

            return 0;
        }

        unique_ptr<heap_stack> get_heap_and_stack_spaces(const int pid)
        {
            procmaps_iterator* maps = pmparser_parse(pid);

            if (maps == nullptr)
            {
                cerr << "cannot parse the memory map of " << pid << endl;
                return nullptr;
            }

            //iterate over areas
            procmaps_struct* map;
            procmaps_struct* previous = nullptr;
            unique_ptr<heap_stack> result(new heap_stack());

            result->anons = unique_ptr<vector<unique_ptr<memory_map>>>(new vector<unique_ptr<memory_map>>());

            while ((map = pmparser_next(maps)) != nullptr && previous != map)
            {
                if (!map->is_r || !map->is_w)
                {
                    previous = map;
                    continue;
                }

                const auto addr_start = reinterpret_cast<off_t>(map->addr_start);
                const auto addr_end = reinterpret_cast<off_t>(map->addr_end);

                if (strcmp(map->pathname, "[heap]") == 0)
                {
                    result->heap = unique_ptr<memory_map>(new memory_map());
                    result->heap->start = addr_start;
                    result->heap->end = addr_end;
                    result->heap->size = addr_end - addr_start;
                }
                else if (strcmp(map->pathname, "[stack]") == 0)
                {
                    result->stack = unique_ptr<memory_map>(new memory_map());
                    result->stack->start = addr_start;
                    result->stack->end = addr_end;
                    result->stack->size = addr_end - addr_start;
                }
                else if (strlen(map->pathname) == 0 && map->is_w)
                {
                    auto anon = unique_ptr<memory_map>(new memory_map());
                    anon->start = addr_start;
                    anon->end = addr_end;
                    anon->size = addr_end - addr_start;
                    result->anons->push_back(move(anon));
                }
                previous = map;
            }

            pmparser_free(maps);

            return result;
        }

        bool is_in_memory(const off_t address, const unique_ptr<heap_stack>& memory_info)
        {
            if (memory_info->heap->start <= address && memory_info->heap->end > address)
            {
                return true;
            }
            if (memory_info->stack->start <= address && memory_info->stack->end > address)
            {
                return true;
            }

            for (auto& anon : *memory_info->anons)
            {
                if (anon->start <= address && anon->end > address)
                {
                    return true;
                }
            }
            return false;
        }
    }
}
