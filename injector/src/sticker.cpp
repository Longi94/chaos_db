#include <random>
#include "sticker.hpp"

using namespace std;

namespace chaos
{
    namespace fault
    {
        BitSticker::BitSticker(cxxopts::ParseResult& args, mt19937& rng) : FaultInjector(args, rng)
        {
            stuck_rate_ = args["stuck-rate"].as<double>();
        }

        void BitSticker::inject(const pid_t pid, atomic_bool& child_running)
        {
            // const auto byte = new int8_t[1];
            //
            // // We get an IO error if we start immediately after the process was created
            // this_thread::sleep_for(chrono::milliseconds(100));
            //
            // while (process::is_child_running(pid))
            // {
            //     if (process::attach(pid))
            //     {
            //         delete[] byte;
            //         return -1;
            //     }
            //
            //     const auto fd = memory::open_mem(pid);
            //
            //     if (memory::read_byte(fd, byte, address))
            //     {
            //         process::detach(pid);
            //         delete[] byte;
            //         return -1;
            //     }
            //
            //     cout << "Read byte: " << bitset<8>(byte[0]) << endl;
            //     byte[0] |= mask; // stick to 1 for now
            //     cout << "Flipped byte: " << bitset<8>(byte[0]) << endl;
            //
            //     if (memory::write_byte(fd, byte, address))
            //     {
            //         process::detach(pid);
            //         delete[] byte;
            //         return -1;
            //     }
            //
            //     close(fd);
            //
            //     process::detach(pid);
            //
            //     this_thread::sleep_for(chrono::milliseconds(500));
            // }
            //
            // delete[] byte;
        }
    }
}