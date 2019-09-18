#include "flipper.hpp"
#include "memory.hpp"
#include <cstdint>
#include <chrono>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono;

namespace chaos
{
    namespace flipper
    {
        int flip_random_bit(const int pid)
        {
            if (memory::attach_to_process(pid))
            {
                return -1;
            }

            const auto byte = new int8_t[1];

            // TODO
            sleep_for(milliseconds(5000));

            memory::detach_from_process(pid);
            delete[] byte;
            return 0;
        }
    }
}
