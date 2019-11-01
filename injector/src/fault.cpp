#include "fault.hpp"
#include "args.hpp"
#include "process.hpp"
#include "cxxopts.hpp"
#include "flipper.hpp"
#include "sticker.hpp"
#include "time.hpp"
#include <random>
#include <chrono>
#include <thread>
#include <csignal>
#include <sys/wait.h>

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

        FaultInjector::FaultInjector(cxxopts::ParseResult& args, mt19937& rng) : rng_(rng)
        {
            inject_space_ = args::get_memory_space(args);
            single_fault_ = args.count("single");

            if (args.count("mean-runtime"))
            {
                mean_runtime_ = args["mean-runtime"].as<long>();
            }
            else
            {
                mean_runtime_ = 0;
            }
        }

        void FaultInjector::inject(const pid_t pid, atomic_bool& stop_flag)
        {
            init_time();
            loop(pid, 100, stop_flag, [](const unique_ptr<memory::heap_stack>& memory_info, const long current_ts)
            {
            });
        }

        void FaultInjector::print_data() const
        {
            process::print_process_status(process_status_);
            cout << "FAULT_COUNT: " << fault_count_ << endl;
            cout << "MAX_HEAP_SIZE: " << max_heap_size_ << endl;
            cout << "MAX_STACK_SIZE: " << max_stack_size_ << endl;
            cout << "TIMEOUT: " << timeout_ << endl;
        }

        void FaultInjector::init_time()
        {
            start_time_ = time::current_time_millis();
            timeout_time_ = start_time_ + 300000; // 10 minute timeout
        }

        bool FaultInjector::check_timeout(const pid_t pid, long& current_timestamp)
        {
            current_timestamp = time::current_time_millis();
            if (current_timestamp > timeout_time_)
            {
                cout << pid << " timeout, killing..." << endl;
                kill(pid, SIGKILL);
                timeout_ = true;
                waitpid(pid, &process_status_, 0);
            }
            return timeout_;
        }

        void FaultInjector::loop(const pid_t pid, const long interval, atomic_bool& stop_flag,
                                 const function<void(const unique_ptr<memory::heap_stack>&, long)> f)
        {
            const chrono::milliseconds sleep_clock(interval);
            this_thread::sleep_for(sleep_clock);

            while (!stop_flag && process_status_ == 0 && process::is_child_running(pid, process_status_))
            {
                long current_ts = 0;
                if (check_timeout(pid, current_ts))
                {
                    continue;
                }

                const auto memory_info = memory::get_heap_and_stack_spaces(pid);

                max_heap_size_ = max(max_heap_size_, memory_info->heap_size);
                max_stack_size_ = max(max_stack_size_, memory_info->stack_size);

                f(memory_info, current_ts);

                this_thread::sleep_for(sleep_clock);
            }

            if (stop_flag)
            {
                kill(pid, SIGKILL);
                waitpid(pid, &process_status_, 0);
            }
        }
    }
}
