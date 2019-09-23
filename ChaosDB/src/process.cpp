#include "process.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cerrno>
#include <iostream>
#include <cstring>

using namespace std;

namespace chaos
{
    namespace process
    {
        int attach(const int pid)
        {
            cout << "ptrace attaching to " << pid << "..." << endl;
            errno = 0;

            if (!ptrace(PTRACE_ATTACH, pid, 0, 0))
            {
                waitpid(pid, nullptr, 0);

                cout << "ptrace attached to " << pid << endl;
                return 0;
            }

            cout << "ptrace attach failed: " << strerror(errno) << endl;
            return -1;
        }

        int detach(const int pid)
        {
            cout << "ptrace detaching from " << pid << "..." << endl;
            errno = 0;

            if (ptrace(PTRACE_DETACH, pid, 0, 0))
            {
                cout << "ptrace detach failed: " << strerror(errno) << endl;
                return -1;
            }

            cout << "ptrace detached from " << pid << endl;
            return 0;
        }
    }
}