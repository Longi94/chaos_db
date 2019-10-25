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
#include <cmath>
#include <sys/wait.h>

using namespace std;

namespace chaos
{
    namespace fault
    {
        BitFlipper::BitFlipper(cxxopts::ParseResult& args, mt19937& rng): FaultInjector(args, rng)
        {
            if (args.count("flip-rate"))
            {
                flip_rate_ = args["flip-rate"].as<double>();
            }
            else
            {
                flip_rate_ = 0;
            }
            random_flip_frequency_ = args.count("random-flip-rate") > 0;
        }

        void BitFlipper::inject(const pid_t pid)
        {
            if (single_fault_)
            {
                // inject a single bit flip at a random time
                uniform_int_distribution<long> runtime_dist(0, mean_runtime_ * 0.75);
                const long delay = runtime_dist(rng_);
                cout << "Injecting bit flip after " << delay << " milliseconds";
                this_thread::sleep_for(chrono::milliseconds(delay));

                const auto memory_info = memory::get_heap_and_stack_spaces(pid);
                flip_random_bit(pid, memory_info, 1);
                waitpid(pid, &process_status_, 0);
                return;
            }

            auto last_flip = start_time_;
            double overflow = 0;

            uniform_real_distribution<double> flip_p_dist(0, 1);

            loop(pid, clock, [this, pid, &flip_p_dist, &overflow, &last_flip](
                 const unique_ptr<memory::heap_stack>& memory_info, const long current_ts)
                 {
                     const auto interval = get_interval(memory_info);

                     if (interval == -1)
                     {
                         // happens when due to the child process dying during parsing the maps file a race condition occurs
                         return;
                     }

                     // cerr << "Interval: " << interval << endl;
                     double p_flip = clock / interval;

                     if (p_flip >= 1.0)
                     {
                         const double n = clock / interval + overflow;
                         const int flip_count = n;
                         overflow = n - flip_count;
                         flip_random_bit(pid, memory_info, flip_count);
                         last_flip = current_ts;
                     }
                     else if (random_flip_frequency_)
                     {
                         if (last_flip == start_time_ && mean_runtime_ > 0 && p_flip < 1.0)
                         {
                             // there were no flips yet, give a probability boost to inject a flip at least once
                             // the boost increases over time, P will reach 1 once the current run-time reaches 3/4 of the mean runtime
                             // cerr << "Increasing probability of a flip" << endl;
                             p_flip += (1.0 - p_flip) * (static_cast<double>(current_ts - start_time_) / (mean_runtime_
                                 * 0.75));
                         }

                         // cerr << "Flip probability: " << p_flip << endl;

                         const auto n = flip_p_dist(rng_);
                         // cerr << "Generated number: " << n << endl;

                         if (n <= p_flip)
                         {
                             flip_random_bit(pid, memory_info, 1);
                             last_flip = current_ts;
                         }
                     }
                     else
                     {
                         if (current_ts - interval > last_flip)
                         {
                             flip_random_bit(pid, memory_info, 1);
                             last_flip = current_ts;
                         }
                     }
                 });
        }

        double BitFlipper::get_interval(const unique_ptr<memory::heap_stack>& memory_info) const
        {
            long mem_size;

            switch (inject_space_)
            {
            case memory::heap:
                mem_size = memory_info->heap_size;
                break;
            case memory::stack:
                mem_size = memory_info->stack_size;
                break;
            default:
                mem_size = memory_info->heap_size + memory_info->stack_size;
                break;
            }

            // cerr << "Mem size: " << mem_size << " bytes" << endl;
            if (mem_size == 0)
            {
                return -1;
            }

            const double interval = 1000000000 / (static_cast<double>(mem_size) * flip_rate_);
            return interval;
        }

        int BitFlipper::flip_random_bit(const pid_t pid, const unique_ptr<memory::heap_stack>& memory_info,
                                        const int flip_count)
        {
            cout << "Flipping " << flip_count << " bits..." << endl;

            int status;
            const int attach_result = process::attach(pid, status);
            if (attach_result)
            {
                if (attach_result == -2)
                {
                    process_status_ = status;
                }
                return -1;
            }

            const auto fd = memory::open_mem(pid);
            const auto byte = new int8_t[1];
            const auto flipped_byte = new int8_t[1];
            uniform_int_distribution<int> mask_dist(0, 7);

            for (int i = 0; i < flip_count; ++i)
            {
                const auto address = get_random_address(memory_info, inject_space_, rng_);
                const auto mask = 1 << mask_dist(rng_);

                if (memory::read_byte(fd, byte, address))
                {
                    process::detach(pid);
                    delete[] byte;
                    return -1;
                }

                // Flip a random bit
                flipped_byte[0] = byte[0] ^ mask;

                cout << "Injected flip: " << hex << address << dec << ", " <<
                    bitset<8>(byte[0]) << " > " << bitset<8>(flipped_byte[0]) << endl;

                if (memory::write_byte(fd, flipped_byte, address))
                {
                    process::detach(pid);
                    delete[] byte;
                    return -1;
                }
            }

            close(fd);

            process::detach(pid);
            delete[] byte;
            delete[] flipped_byte;
            fault_count_ += flip_count;
            return 0;
        }
    }
}
