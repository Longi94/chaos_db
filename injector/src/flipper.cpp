#include "flipper.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "args.hpp"
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
        unique_ptr<FaultInjector> get_injector(const fault_type fault_type, cxxopts::ParseResult& args, mt19937& rng)
        {
            switch (fault_type)
            {
            case flip:
                return unique_ptr<FaultInjector>(new BitFlipper(args, rng));
            case stuck:
                return unique_ptr<FaultInjector>(new BitSticker(args, rng));
            default:
                return nullptr;
            }
        }

        FaultInjector::FaultInjector(cxxopts::ParseResult& args, std::mt19937& rng) : rng_(rng)
        {
            inject_space_ = args::get_memory_space(args);
        }

        BitFlipper::BitFlipper(cxxopts::ParseResult& args, mt19937& rng): FaultInjector(args, rng)
        {
            flip_rate_ = args["flip-rate"].as<double>();
        }

        BitSticker::BitSticker(cxxopts::ParseResult& args, mt19937& rng): FaultInjector(args, rng)
        {
            stuck_rate_ = args["stuck-rate"].as<double>();
        }

        int BitFlipper::inject(const pid_t pid)
        {

            auto last_flip = chrono::duration_cast<chrono::milliseconds>(
                chrono::system_clock::now().time_since_epoch()
            );

            this_thread::sleep_for(chrono::milliseconds(100));

            while (process::is_child_running(pid))
            {
                const auto current_ts = chrono::duration_cast<chrono::milliseconds>(
                    chrono::system_clock::now().time_since_epoch()
                );

                auto interval = get_interval(pid);
                if (interval.count() < 100)
                {
                    // don't inject too frequently
                    interval = chrono::milliseconds(100);
                }

                if (current_ts - interval > last_flip)
                {
                    flip_random_bit(pid);
                    last_flip = current_ts;
                }
                this_thread::sleep_for(chrono::milliseconds(100));
            }
            return 0;
        }

        std::chrono::milliseconds BitFlipper::get_interval(const pid_t pid) const
        {
            const auto heap_stack = memory::get_heap_and_stack_spaces(pid);

            long mem_size;

            switch (inject_space_)
            {
            case memory::heap:
                mem_size = heap_stack->heap_end - heap_stack->heap_start;
                break;
            case memory::stack:
                mem_size = heap_stack->stack_end - heap_stack->stack_start;
                break;
            default:
                mem_size = heap_stack->heap_end - heap_stack->heap_start + heap_stack->stack_end - heap_stack->
                    stack_start;
                break;
            }

            const long interval = 1000000000 * flip_rate_ / static_cast<double>(mem_size);
            return chrono::milliseconds(interval);
        }

        int BitFlipper::flip_random_bit(const pid_t pid)
        {
            const auto address = get_random_address(pid, inject_space_, rng_);

            uniform_int_distribution<int> mask_dist(0, 7);
            const auto mask = 1 << mask_dist(rng_);

            cout << "Chosen address: " << hex << address << dec << endl;
            cout << "Inject mask: " << bitset<8>(mask) << endl;

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

        int BitSticker::inject(const pid_t pid)
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
            return 0;
        }
    }
}
