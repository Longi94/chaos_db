#include "flipper.hpp"
#include "memory.hpp"
#include <cstdint>
#include <chrono>
#include <thread>
#include <iostream>

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

            cout << "Read byte: " << static_cast<int16_t>(byte[0]) << endl;

            memory::detach_from_process(pid);
            delete[] byte;
            return 0;
        }
    }
}
