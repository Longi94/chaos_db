#pragma once
#include <unistd.h>
#include "memory.hpp"

namespace chaos
{
    namespace flipper
    {
        enum fault_type { none, flip, stuck };

        /**
         * Flip a bit in the memory of a process.
         * @param pid The id of the process whose memory will be touched.
         * @param offset The address of the byte to flip bit in. If -1 a random address will be chosen.
         * @param m_space The memory space to inject the bit flip into.
         * @return 0 if successful, -1 if there was an error
         */
        int flip_random_bit(int pid, off_t offset, memory::space m_space);

        class Flipper
        {
        public:
            virtual int inject(pid_t pid, off_t address) = 0;
        };

        class BitFlipper : public Flipper
        {
        public:
            int inject(pid_t pid, off_t address);
        };

        class BitSticker : public Flipper
        {
        public:
            int inject(pid_t pid, off_t address);
        };

        /**
         * Get the appropriate fault injector class.
         * @param fault_type the type of the fault that needs to be injected
         */
        Flipper* get_flipper(fault_type fault_type);
    }
}
