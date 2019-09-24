#pragma once
#include <unistd.h>

namespace chaos
{
    namespace flipper
    {
        /**
         * Flip a bit in the memory of a process.
         * @param pid The id of the process whose memory will be touched.
         * @param offset The address of the byte to flip bit in. If -1 a random address will be chosen.
         * @return 0 if successful, -1 if there was an error
         */
        int flip_random_bit(int pid, off_t offset);
    }
}
