#include "flipper.hpp"
#include "memory.hpp"
#include "process.hpp"
#include <cstdint>
#include <iostream>
#include <random>
#include <bitset>
#include <fcntl.h>

using namespace std;

namespace chaos
{
    namespace flipper
    {
        int flip_random_bit(const int pid, const off_t offset, const memory::space m_space)
        {
            if (process::attach(pid))
            {
                return -1;
            }

            off_t addr = offset;

            if (addr < 0)
            {
                addr = memory::get_random_address(pid, m_space);
            }

            cout << "Chosen address: " << hex << addr << dec << endl;

            const auto byte = new int8_t[1];

            const auto fd = memory::open_mem(pid);

            if (memory::read_byte(fd, byte, addr))
            {
                return -1;
            }

            cout << "Read byte: " << bitset<8>(byte[0]) << endl;

            // Flip a random bit
            byte[0] ^= 1 << rand() % 7;
            cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;

            if (memory::write_byte(fd, byte, addr))
            {
                return -1;
            }

            close(fd);

            process::detach(pid);
            delete[] byte;
            return 0;
        }

        FaultInjector* get_injector(const fault_type fault_type)
        {
            switch (fault_type)
            {
                case flip:
                    return new BitFlipper();
                case stuck:
                    return new BitSticker();
                default:
                    return nullptr;
            }
        }

        int BitFlipper::inject(const pid_t pid, const off_t address)
        {
            const auto byte = new int8_t[1];

            const auto fd = memory::open_mem(pid);

            if (memory::read_byte(fd, byte, address))
            {
                return -1;
            }

            cout << "Read byte: " << bitset<8>(byte[0]) << endl;

            // Flip a random bit
            byte[0] ^= 1 << rand() % 7;
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

        int BitSticker::inject(const pid_t pid, const off_t address)
        {
            return 0;
        }
    }
}
