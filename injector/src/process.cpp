#include "process.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

constexpr int excl_fail = 127;

namespace chaos
{
    namespace process
    {
        int attach(const pid_t pid)
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

        int detach(const pid_t pid)
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

        pid_t execute(string& path, string& output, string& input, string& error, char** arguments)
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
                    if (fdout == -1)
                    {
                        cerr << "Failed to open fdout " << output << ": " << strerror(errno) << endl;
                    }
                    else
                    {
                        dup2(fdout, pipe_write);
                        close(fdout);
                    }

                    // redirect stderr to file
                    if (!error.empty())
                    {
                        const int fderr = open(error.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                        if (fdout == -1)
                        {
                            cerr << "Failed to open fderr " << error << ": " << strerror(errno) << endl;
                        }
                        else
                        {
                            dup2(fderr, pipe_error);
                            close(fderr);
                        }
                    }

                    // pipe file into stdin
                    if (!input.empty())
                    {
                        const int fdin = open(input.c_str(), O_RDONLY);
                        if (fdout == -1)
                        {
                            cerr << "Failed to open fdin " << input << ": " << strerror(errno) << endl;
                        }
                        else
                        {
                            dup2(fdin, pipe_read);
                            close(fdin);
                        }
                    }

                    if (execv(path.c_str(), arguments) == -1)
                    {
                        cerr << strerror(errno) << endl;
                        perror("execl");
                        exit(excl_fail);
                    }
                    return 0;
                }
            default:
                cout << "Child process id from parent: " << pid << endl;
                return pid;
            }
        }

        void print_process_status(const int status)
        {
            cout << "WIFEXITED: " << WIFEXITED(status) << endl;
            cout << "WEXITSTATUS: " << WEXITSTATUS(status) << endl;
            cout << "WIFSIGNALED: " << WIFSIGNALED(status) << endl;
            cout << "WTERMSIG: " << WTERMSIG(status) << endl;
            cout << "WCOREDUMP: " << WCOREDUMP(status) << endl;
            cout << "WIFSTOPPED: " << WIFSTOPPED(status) << endl;
            cout << "WSTOPSIG: " << WSTOPSIG(status) << endl;
        }

        bool is_child_running(const pid_t pid, int& status)
        {
            const pid_t result = waitpid(pid, &status, WNOHANG);
            return result == 0;
        }
    }
}
