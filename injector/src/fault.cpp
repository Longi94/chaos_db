#include "fault.hpp"
#include "args.hpp"
#include "process.hpp"
#include "cxxopts.hpp"
#include "flipper.hpp"
#include "sticker.hpp"
#include <random>
#include <chrono>
#include <thread>

using namespace std;

namespace chaos
{
    namespace fault
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
                return unique_ptr<FaultInjector>(new FaultInjector(args, rng));
            }
        }

        FaultInjector::FaultInjector(cxxopts::ParseResult& args, std::mt19937& rng) : rng_(rng)
        {
            inject_space_ = args::get_memory_space(args);

            if (args.count("mean-runtime"))
            {
                mean_runtime_ = args["mean-runtime"].as<long>();
            }
            else
            {
                mean_runtime_ = 0;
            }
        }

        int FaultInjector::inject(const pid_t pid)
        {
            const chrono::milliseconds sleep_clock(100);
            this_thread::sleep_for(sleep_clock);
            while (process::is_child_running(pid))
            {
                const auto memory_info = memory::get_heap_and_stack_spaces(pid);

                max_heap_size_ = max(max_heap_size_, memory_info->heap_size);
                max_stack_size_ = max(max_stack_size_, memory_info->stack_size);
                this_thread::sleep_for(sleep_clock);
            }

            return 0;
        }

        void FaultInjector::print_data() const
        {
            cout << "MAX_HEAP_SIZE: " << max_heap_size_ << endl;
            cout << "MAX_STACK_SIZE: " << max_stack_size_ << endl;
        }
    }
}
