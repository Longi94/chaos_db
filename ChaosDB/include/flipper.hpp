#pragma once
#include <unistd.h>

namespace chaos
{
    namespace flipper
    {
        int flip_random_bit(int pid, off_t offset);
    }
}
