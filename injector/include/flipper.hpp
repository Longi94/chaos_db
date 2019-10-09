#pragma once
#include "fault.hpp"
#include <unistd.h>
#include <memory>
#include <random>
#include <chrono>
#include "cxxopts.hpp"

namespace chaos
{
    namespace flipper
    {

        class BitFlipper : public fault::FaultInjector
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
             * @param pid child process id
             * @return time interval of the flip frequency in milliseconds
             */
            long get_interval(pid_t pid) const;

            /**
             * Flip a random bit in the child process' memory. The address and the bit is randomly chosen using rng_.
             * @param pid child process id
             */
            int flip_random_bit(pid_t pid);
        };

        class BitSticker : public fault::FaultInjector
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
        std::unique_ptr<fault::FaultInjector> get_injector(fault::fault_type fault_type, cxxopts::ParseResult& args, std::mt19937& rng);
    }
}
