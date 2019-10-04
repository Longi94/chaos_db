#include "flipper.hpp"
#include "memory.hpp"
#include "process.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <bitset>
#include <thread>
#include <chrono>

using namespace std;

namespace chaos
{
    namespace flipper
    {
        unique_ptr<FaultInjector> get_injector(const fault_type fault_type, mt19937 &rng)
        {
            switch (fault_type)
            {
                case flip:
                    return unique_ptr<FaultInjector>(new BitFlipper(rng));
                case stuck:
                    return unique_ptr<FaultInjector>(new BitSticker(rng));
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
                process::detach(pid);
                delete[] byte;
                return -1;
            }

            cout << "Read byte: " << bitset<8>(byte[0]) << endl;

            // Flip a random bit
            byte[0] ^= mask;
            cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;

            if (memory::write_byte(fd, byte, address))
            {
                process::detach(pid);
                delete[] byte;
                return -1;
            }

            close(fd);

            process::detach(pid);
            delete[] byte;
            return 0;
        }

        int BitSticker::inject(const pid_t pid, const off_t address, const int8_t mask)
        {
            const auto byte = new int8_t[1];

            // We get an IO error if we start immediately after the process was created
            this_thread::sleep_for(chrono::milliseconds(100));

            while (process::is_child_running(pid))
            {
                if (process::attach(pid))
                {
                    delete[] byte;
                    return -1;
                }

                const auto fd = memory::open_mem(pid);

                if (memory::read_byte(fd, byte, address))
                {
                    process::detach(pid);
                    delete[] byte;
                    return -1;
                }

                cout << "Read byte: " << bitset<8>(byte[0]) << endl;
                byte[0] |= mask; // stick to 1 for now
                cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;

                if (memory::write_byte(fd, byte, address))
                {
                    process::detach(pid);
                    delete[] byte;
                    return -1;
                }

                close(fd);

                process::detach(pid);

                this_thread::sleep_for(chrono::milliseconds(500));
            }
            
            delete[] byte;
            return 0;
        }
    }
}
