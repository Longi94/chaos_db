#include "flipper.hpp"
#include "memory.hpp"
#include <cstdint>
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <bitset>
#include <fcntl.h>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

namespace chaos
{
    namespace flipper
    {
        int flip_random_bit(const int pid, const off_t offset)
        {
            if (memory::attach_to_process(pid))
            {
                return -1;
            }

            off_t addr = offset;

            if (addr < 0)
            {
                memory::heap_stack* memory_info = memory::get_heap_and_stack_spaces(pid);

                if (memory_info != nullptr)
                {
                    cout << "Heap: " << hex << memory_info->heap_start << "-" << memory_info->heap_end << " " <<
                        dec << memory_info->heap_end - memory_info->heap_start << endl;

                    cout << "Stack: " << hex << memory_info->stack_start << "-" << memory_info->stack_end << " " <<
                        dec << memory_info->stack_end - memory_info->stack_start << endl;

                    const auto heap_size = memory_info->heap_end - memory_info->heap_start;
                    const auto stack_size = memory_info->stack_end - memory_info->stack_start;

                    // Randomly choose an address in heap or stack
                    const int rand_i = rand() % (heap_size + stack_size);

                    if (rand_i > heap_size)
                    {
                        addr = memory_info->stack_start - heap_size + rand_i;
                    }
                    else
                    {
                        addr = memory_info->heap_start + rand_i;
                    }
                }
            }

            cout << "Chosen address: " << hex << addr << dec << endl;

            const auto byte = new int8_t[1];

            const auto fd = memory::open_mem(pid);

            memory::read_byte(fd, byte, addr);

            cout << "Read byte: " << bitset<8>(byte[0]) << endl;

            // Flip a random bit
            byte[0] ^= 1 << rand() % 7;
            cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;

            //memory::write_byte(fd, byte, offset);

            close(fd);

            memory::detach_from_process(pid);
            delete[] byte;
            return 0;
        }
    }
}
