#pragma once

namespace chaos
{
    namespace process
    {
        int detach(int pid);
        int attach(int pid);
    }
}
