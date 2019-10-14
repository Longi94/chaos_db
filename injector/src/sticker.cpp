#include <random>
#include <thread>
#include <chrono>
#include "process.hpp"
#include "sticker.hpp"
#include "memory.hpp"

using namespace std;

namespace chaos
{
    namespace fault
    {
        BitSticker::BitSticker(cxxopts::ParseResult& args, mt19937& rng) : FaultInjector(args, rng)
        {
            stuck_rate_ = args["stuck-rate"].as<double>();
            stuck_bits_.reserve(10);
        }

        void BitSticker::inject(const pid_t pid)
        {
            const auto byte = new int8_t[1];

            // We get an IO error if we start immediately after the process was created
            const chrono::milliseconds sleep_clock(100);
            this_thread::sleep_for(sleep_clock);

            while (process::is_child_running(pid, process_status_))
            {
                const auto memory_info = memory::get_heap_and_stack_spaces(pid);

                if (process::attach(pid))
                {
                    delete[] byte;
                    return;
                }

                const int fd = memory::open_mem(pid);

                for (auto stuck_bit : stuck_bits_)
                {
                    if (memory::read_byte(fd, byte, stuck_bit.address))
                    {
                        process::detach(pid);
                        delete[] byte;
                        return;
                    }

                    cout << "Read byte: " << bitset<8>(byte[0]) << endl;
                    cout << "Mask: " << bitset<8>(stuck_bit.mask) << endl;

                    if (stuck_bit.stick_to_1)
                    {
                        byte[0] |= stuck_bit.mask;
                    }
                    else
                    {
                        byte[0] &= stuck_bit.mask;
                    }

                    cout << "Stuck byte: " << bitset<8>(byte[0]) << endl;

                    if (memory::write_byte(fd, byte, stuck_bit.address))
                    {
                        process::detach(pid);
                        delete[] byte;
                        return;
                    }
                }

                close(fd);

                process::detach(pid);

                this_thread::sleep_for(sleep_clock);
            }

            delete[] byte;
        }
    }
}