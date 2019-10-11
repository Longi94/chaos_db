#include "flipper.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "time.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <random>
#include <bitset>
#include <thread>
#include <chrono>
#include <cmath>

using namespace std;

namespace chaos
{
    namespace fault
    {
        BitFlipper::BitFlipper(cxxopts::ParseResult& args, mt19937& rng): FaultInjector(args, rng)
        {
            flip_rate_ = args["flip-rate"].as<double>();
            random_flip_frequency_ = args.count("random-flip-rate") > 0;
        }

        void BitFlipper::inject(const pid_t pid)
        {
            const long start_ts = time::current_time_millis();
            auto last_flip = start_ts;

            const chrono::milliseconds sleep_clock(clock);
            uniform_real_distribution<double> flip_p_dist(0, 1);

            this_thread::sleep_for(sleep_clock);

            while (process::is_child_running(pid, process_status_))
            {
                const auto memory_info = memory::get_heap_and_stack_spaces(pid);

                max_heap_size_ = max(max_heap_size_, memory_info->heap_size);
                max_stack_size_ = max(max_stack_size_, memory_info->stack_size);

                const auto interval = get_interval(memory_info);
                const long current_ts = time::current_time_millis();

                cerr << "Interval: " << interval << endl;

                if (random_flip_frequency_)
                {
                    double p_flip = clock / static_cast<double>(interval);

                    if (last_flip == start_ts && mean_runtime_ > 0 && p_flip < 1.0)
                    {
                        // there were no flips yet, give a probability boost to inject a flip at least once
                        // the boost increases over time, P will reach 1 once the current run-time reaches 3/4 of the mean runtime
                        cerr << "Increasing probability of a flip" << endl;
                        p_flip += (1.0 - p_flip) * (static_cast<double>(current_ts - start_ts) / (mean_runtime_ * 0.75));
                    }

                    cerr << "Flip probability: " << p_flip << endl;

                    const auto n = flip_p_dist(rng_);
                    cerr << "Generated number: " << n << endl;

                    if (n <= p_flip)
                    {
                        flip_random_bit(pid, memory_info);
                        last_flip = current_ts;
                    }
                }
                else
                {
                    if (current_ts - interval > last_flip)
                    {
                        flip_random_bit(pid, memory_info);
                        last_flip = current_ts;
                    }
                }

                this_thread::sleep_for(sleep_clock);
            }
        }

        long BitFlipper::get_interval(const unique_ptr<memory::heap_stack>& memory_info) const
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

            cerr << "Mem size: " << mem_size << " bytes" << endl;

            const long interval = 1000000000 / (static_cast<double>(mem_size) * flip_rate_);
            return interval;
        }

        int BitFlipper::flip_random_bit(const pid_t pid, const unique_ptr<memory::heap_stack>& memory_info)
        {
            const auto address = get_random_address(memory_info, inject_space_, rng_);

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
    }
}
