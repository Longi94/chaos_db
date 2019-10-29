#pragma once
#include <atomic>
#include <thread>
#include <condition_variable>

namespace chaos
{
    namespace server
    {
        constexpr char message_start = 1;
        constexpr char message_stop = 2;
        constexpr int buffer_size = 16;

        void start(int port, std::condition_variable& start_flag, std::atomic_bool& stop_flag);
        std::unique_ptr<std::thread> start_background(int port, std::condition_variable& start_flag, std::atomic_bool& stop_flag);
    }
}
