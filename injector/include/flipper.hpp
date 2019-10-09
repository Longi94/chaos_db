#pragma once
#include <unistd.h>
#include <random>
#include "fault.hpp"
#include "cxxopts.hpp"

namespace chaos
{
    namespace fault
    {
        class BitFlipper : public FaultInjector
        {
        public:
            BitFlipper(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            static const long clock = 100;

            double flip_rate_;
            bool random_flip_frequency_;

        public:
            int inject(pid_t pid) override;
        private:
            /**
             * Calculates how frequently bit flips should happen based on flip_rate_ and the currently allocated memory
             * size of the child process.
             * @param memory_info memory info of the child process
             * @return time interval of the flip frequency in milliseconds
             */
            long get_interval(const std::unique_ptr<memory::heap_stack>& memory_info) const;

            /**
             * Flip a random bit in the child process' memory. The address and the bit is randomly chosen using rng_.
             * @param pid child process id
             * @param memory_info memory info of the child process
             */
            int flip_random_bit(pid_t pid, const std::unique_ptr<memory::heap_stack>& memory_info);
        };
    }
}
