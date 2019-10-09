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
            long mean_runtime_;
            long max_heap_size_ = 0;
            long max_stack_size_ = 0;

        public:
            /**
             * Inject the fault into the memory.
             * @param pid the id of the process whose memory will be tinkered with
             * @param child_running
             */
            virtual void inject(pid_t pid, std::atomic_bool& child_running);
            void print_data() const;
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         * @param rng random number generator used for randomly selecting an address
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type, cxxopts::ParseResult& args, std::mt19937& rng);
    }
}
