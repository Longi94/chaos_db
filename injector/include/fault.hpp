#pragma once
#include <unistd.h>
#include <memory>
#include <random>
#include "cxxopts.hpp"
#include "memory.hpp"

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

        public:
            /**
             * Inject the fault into the memory.
             * @param pid the id of the process whose memory will be tinkered with
             */
            virtual int inject(pid_t pid) = 0;
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         * @param rng random number generator used for randomly selecting an address
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type, cxxopts::ParseResult& args, std::mt19937& rng);
    }
}
