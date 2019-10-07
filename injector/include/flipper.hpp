#pragma once
#include <unistd.h>
#include <cstdint>
#include <memory>
#include <random>
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
             * @param address of the byte
             * @param mask the byte mask
             */
            virtual int inject(pid_t pid, off_t address, int8_t mask) = 0;
        };

        class BitFlipper : public FaultInjector
        {
        public:
            BitFlipper(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            float flip_rate_;

        public:
            int inject(pid_t pid, off_t address, int8_t mask) override;
        };

        class BitSticker : public FaultInjector
        {
        public:
            BitSticker(cxxopts::ParseResult& args, std::mt19937& rng);

        private:
            float stuck_rate_;

        public:
            int inject(pid_t pid, off_t address, int8_t mask) override;
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         * @param rng random number generator used for randomly selecting an address
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type, cxxopts::ParseResult& args, std::mt19937& rng);
    }
}
