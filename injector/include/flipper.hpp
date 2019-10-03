#pragma once
#include <unistd.h>
#include <cstdint>
#include <memory>
#include "memory.hpp"

namespace chaos
{
    namespace flipper
    {
        enum fault_type { none, flip, stuck };

        class FaultInjector
        {
        public:
            /**
             * Inject the fault into the memory.
             * @param pid the id of the process whose memory will be tinkered with
             * @param address of the byte
             * @param the byte mask
             */
            virtual int inject(pid_t pid, off_t address, int8_t mask) = 0;
        };

        class BitFlipper : public FaultInjector
        {
        public:
            int inject(pid_t pid, off_t address, int8_t mask);
        };

        class BitSticker : public FaultInjector
        {
        public:
            int inject(pid_t pid, off_t address, int8_t mask);
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         */
        std::unique_ptr<FaultInjector> get_injector(fault_type fault_type);
    }
}
