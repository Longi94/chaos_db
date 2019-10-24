#pragma once
#include <unistd.h>
#include <memory>
#include <random>
#include "cxxopts.hpp"
#include "memory.hpp"
#include <atomic>
#include <thread>

namespace chaos
{
    namespace fault
    {
        enum fault_type { none, flip, stuck };

        class FaultInjector
        {
        public:
            explicit FaultInjector(cxxopts::ParseResult& args, std::mt19937& rng);

        protected:
            std::mt19937 rng_;
            memory::space inject_space_;
            bool single_fault_;
            int process_status_ = 0;
            int fault_count_ = 0;
            long mean_runtime_;
            long max_heap_size_ = 0;
            long max_stack_size_ = 0;

            long start_time_ = 0;
            long timeout_time_ = 0;
            bool timeout_ = false;

        public:
            /**
             * Inject the fault into the memory.
             * @param pid the id of the process whose memory will be tinkered with
             */
            virtual void inject(pid_t pid);
            void print_data() const;

        protected:
            void init_time();
            bool check_timeout(pid_t pid, long& current_timestamp);
            void loop(pid_t pid, long interval, std::function<void(const std::unique_ptr<memory::heap_stack>&, long)> f);
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         * @param rng random number generator used for randomly selecting an address
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type, cxxopts::ParseResult& args, std::mt19937& rng);
    }
}
