#pragma once
#include <unistd.h>
#include <cstdint>
#include <memory>
#include <random>

namespace chaos
{
    namespace flipper
    {
        enum fault_type { none, flip, stuck };

        class FaultInjector
        {
        public:
            std::mt19937 rng;

            explicit FaultInjector(std::mt19937 &rng): rng(rng)
            {
            }

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
            explicit BitFlipper(std::mt19937 &rng): FaultInjector(rng)
            {
            }

            int inject(pid_t pid, off_t address, int8_t mask);
        };

        class BitSticker : public FaultInjector
        {
        public:
            explicit BitSticker(std::mt19937 &rng) : FaultInjector(rng)
            {
            }

            int inject(pid_t pid, off_t address, int8_t mask);
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         * @param rng random number generator used for randomly selecting an address
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type, std::mt19937 &rng);
    }
}
