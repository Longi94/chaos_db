#include "flipper.hpp"
#include "memory.hpp"
#include <cstdint>
#include <chrono>
#include <thread>
#include <iostream>
#include <random>
#include <bitset>

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

            const auto byte = new int8_t[1];

            const auto fd = memory::open_mem(pid);

            memory::read_byte(fd, byte, offset);

            cout << "Read byte: " << bitset<8>(byte[0]) << endl;

            // Flip a random bit
            byte[0] ^= 1 << rand() % 7;
            cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;

            memory::detach_from_process(pid);
            delete[] byte;
            return 0;
        }
    }
}
