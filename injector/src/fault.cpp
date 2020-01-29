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
            single_fault_ = args.count("single");
            inject_to_heap_ = args.count("heap");
            inject_to_anon_ = args.count("anonymous");
            inject_to_stack_ = args.count("stack");

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

        void FaultInjector::get_result(unique_ptr<result>& result) const
        {
            result->exited = WIFEXITED(process_status_);
            result->return_code = WEXITSTATUS(process_status_);
            result->signaled = WIFSIGNALED(process_status_);
            result->term_sig = WTERMSIG(process_status_);
            result->fault_count = fault_count_;
            result->max_heap_size = max_heap_size_;
            result->max_stack_size = max_stack_size_;
            result->timeout = timeout_;
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

                max_heap_size_ = max(max_heap_size_, memory_info->heap->size);
                max_stack_size_ = max(max_stack_size_, memory_info->stack->size);

                f(memory_info, current_ts);

                this_thread::sleep_for(sleep_clock);
            }

            if (stop_flag)
            {
                kill(pid, SIGKILL);
                waitpid(pid, &process_status_, 0);
            }
        }

        long FaultInjector::get_total_memory_size(const std::unique_ptr<memory::heap_stack>& memory_info) const
        {
            long total_size = 0;

            if (inject_to_heap_)
            {
                total_size += memory_info->heap->size;
            }
            if (inject_to_stack_)
            {
                total_size += memory_info->stack->size;
            }
            if (inject_to_anon_)
            {
                for (const auto& a : *memory_info->anons)
                {
                    total_size += a->size;
                }
            }

            return total_size;
        }

        off_t FaultInjector::get_random_address(const unique_ptr<memory::heap_stack>& memory_info,
                                                const long total_size)
        {
            if (memory_info == nullptr || total_size == 0)
            {
                return 0;
            }

            uniform_int_distribution<long> address_dist(0, total_size);
            auto selected = address_dist(rng_);

            if (inject_to_heap_)
            {
                if (memory_info->heap->size > selected)
                {
                    return memory_info->heap->start + selected;
                }
                selected -= memory_info->heap->size;
            }
            if (inject_to_stack_)
            {
                if (memory_info->stack->size > selected)
                {
                    return memory_info->stack->start + selected;
                }
                selected -= memory_info->stack->size;
            }
            if (inject_to_anon_)
            {
                for (const auto& a : *memory_info->anons)
                {
                    if (a->size > selected)
                    {
                        return a->start + selected;
                    }
                    selected -= a->size;
                }
            }

            return 0;
        }
    }
}
