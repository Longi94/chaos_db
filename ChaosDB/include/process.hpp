#pragma once
#include <unistd.h>
#include <string>

constexpr auto pipe_read = 0;
constexpr auto pipe_write = 1;

using namespace std;

namespace chaos
{
    namespace process
    {
        int detach(int pid);
        int attach(int pid);
        pid_t execute(string command, string output, string input, char** arguments);
    }
}
