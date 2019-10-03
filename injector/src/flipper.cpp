#include "flipper.hpp"
#include "memory.hpp"
#include "process.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <bitset>
#include <fcntl.h>

using namespace std;

namespace chaos
{
    namespace flipper
    {
        unique_ptr<FaultInjector> get_injector(const fault_type fault_type)
        {
            switch (fault_type)
            {
                case flip:
                    return unique_ptr<FaultInjector>(new BitFlipper());
                case stuck:
                    return unique_ptr<FaultInjector>(new BitSticker());
                default:
                    return nullptr;
            }
        }

        int BitFlipper::inject(const pid_t pid, const off_t address, const int8_t mask)
        {
            if (process::attach(pid))
            {
                return -1;
            }

            const auto byte = new int8_t[1];

            const auto fd = memory::open_mem(pid);

            if (memory::read_byte(fd, byte, address))
            {
                return -1;
            }

            cout << "Read byte: " << bitset<8>(byte[0]) << endl;

            // Flip a random bit
            byte[0] ^= mask;
            cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;

            if (memory::write_byte(fd, byte, address))
            {
                return -1;
            }

            close(fd);

            process::detach(pid);
            delete[] byte;
            return 0;
        }

        int BitSticker::inject(const pid_t pid, const off_t address, const int8_t mask)
        {
            return 0;
        }
    }
}
