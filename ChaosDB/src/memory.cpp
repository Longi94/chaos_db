#include "memory.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
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
        int attach_to_process(const int pid)
        {
            cout << "ptrace attaching to " << pid << "..." << endl;
            errno = 0;

            if (!ptrace(PTRACE_ATTACH, pid, 0, 0))
            {
                waitpid(pid, nullptr, 0);

                cout << "ptrace attached to " << pid << endl;
                return 0;
            }

            cout << "ptrace attach failed: " << strerror(errno) << endl;
            return -1;
        }

        int detach_from_process(const int pid)
        {
            cout << "ptrace detaching from " << pid << "..." << endl;
            errno = 0;

            if (ptrace(PTRACE_DETACH, pid, 0, 0))
            {
                cout << "ptrace detach failed: " << strerror(errno) << endl;
                return -1;
            }

            cout << "ptrace detached from " << pid << endl;
            return 0;
        }

        int open_mem(const int pid)
        {
            char file[64];
            sprintf(file, "/proc/%d/mem", pid);
            return open(file, O_RDWR);
        }

        void read_byte(const int fd, void* buf, const off_t address)
        {
            pread(fd, buf, 1, address);
        }

        void write_byte(const int fd, void* buf, const off_t address)
        {
            pwrite(fd, buf, 1, address);
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
