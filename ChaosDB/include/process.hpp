#pragma once
#include <unistd.h>
#include <vector>
#include <string>

using namespace std;

namespace chaos
{
    namespace process
    {
        int detach(int pid);
        int attach(int pid);
        pid_t execute(string command, string output, char** arguments);
    }
}
