#pragma once
#include <unistd.h>
#include <cstdint>
#include <memory>
#include <random>
#include <chrono>
#include "cxxopts.hpp"
#include "memory.hpp"

namespace chaos
{
    namespace flipper
    {
        enum fault_type { none, flip, stuck };

        class FaultInjector
        {
        public:
            explicit FaultInjector(cxxopts::ParseResult& args, std::mt19937& rng);

        protected:
            std::mt19937 rng_;
            memory::space inject_space_;

        public:
            /**
             * Inject the fault into the memory.
             * @param pid the id of the process whose memory will be tinkered with
             */
            virtual int inject(pid_t pid) = 0;
        };

        class BitFlipper : public FaultInjector
        {
        public:
            BitFlipper(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            double flip_rate_;

        public:
            int inject(pid_t pid) override;
        private:
            /**
             * Calculates how frequently bit flips should happen based on flip_rate_ and the currently allocated memory
             * size of the child process.
             * @param pid child process id
             * @return time interval of the flip frequency in milliseconds
             */
            std::chrono::milliseconds get_interval(pid_t pid) const;

            /**
             * Flip a random bit in the child process' memory. The address and the bit is randomly chosen using rng_.
             * @param pid child process id
             */
            int flip_random_bit(pid_t pid);
        };

        class BitSticker : public FaultInjector
        {
        public:
            BitSticker(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            double stuck_rate_;

        public:
            int inject(pid_t pid) override;
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         * @param rng random number generator used for randomly selecting an address
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type, cxxopts::ParseResult& args, std::mt19937& rng);
    }
}
