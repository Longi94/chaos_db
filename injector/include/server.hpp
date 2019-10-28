#pragma once
#include <atomic>
#include <thread>

namespace chaos
{
    namespace server
    {
        void start(int port, std::atomic_bool& start_flag, std::atomic_bool& stop_flag);
        std::unique_ptr<std::thread> start_background(int port, std::atomic_bool& start_flag, std::atomic_bool& stop_flag);
    }
}
