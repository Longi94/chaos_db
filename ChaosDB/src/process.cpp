#include "process.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

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

        pid_t execute(string path, string output, string input, char** arguments)
        {
            cout << "Forking to run:";
            for (char** p = arguments; *p != nullptr; p++)
            {
                cout << " " << *p;
            }
            cout << endl;

            const pid_t pid = fork();

            switch (pid)
            {
            case -1:
                return -1;
            case 0:
                {
                    // redirect stdout to file
                    const int fdout = open(output.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    dup2(fdout, pipe_write);
                    close(fdout);

                    // pipe file into stdin
                    if (!input.empty())
                    {
                        const int fdin = open(input.c_str(), O_RDONLY);
                        dup2(fdin, pipe_read);
                        close(fdin);
                    }

                    execv(path.c_str(), arguments);
                    perror("execl");
                    exit(1);
                    return 0;
                }
            default:
                cout << "Child process id from parent: " << pid << endl;
                return pid;
            }
        }
    }
}
