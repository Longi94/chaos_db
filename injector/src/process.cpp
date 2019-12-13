#include "process.hpp"
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cerrno>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <thread>

using namespace std;

constexpr int excl_fail = 127;

namespace chaos
{
    namespace process
    {
        int attach(const pid_t pid, int& status)
        {
            // cout << "ptrace attaching to " << pid << "..." << endl;
            errno = 0;

            if (!ptrace(PTRACE_ATTACH, pid, 0, 0))
            {
                waitpid(pid, &status, 0);

                if (!WIFSTOPPED(status))
                {
                    cout << "ptrace attach failed: process not stopped" << endl;
                    return -2;
                }

                // cout << "ptrace attached to " << pid << endl;
                return 0;
            }

            cout << "ptrace attach failed: " << strerror(errno) << endl;
            return -1;
        }

        int detach(const pid_t pid)
        {
            // cout << "ptrace detaching from " << pid << "..." << endl;
            errno = 0;

            if (ptrace(PTRACE_DETACH, pid, 0, 0))
            {
                cout << "ptrace detach failed: " << strerror(errno) << endl;
                return -1;
            }

            // cout << "ptrace detached from " << pid << endl;
            return 0;
        }

        void execute(string& path, string& input, char** arguments, string& stdout, string& stderr, pid_t& pid)
        {
            cout << "Forking to run:";
            for (char** p = arguments; *p != nullptr; p++)
            {
                cout << " " << *p;
            }
            cout << endl;

            int stdout_link[2];
            int stderr_link[2];
            pipe(stdout_link);
            pipe(stderr_link);

            pid = fork();

            switch (pid)
            {
            case -1:
                return;
            case 0:
                {
                    dup2(stdout_link[1], STDOUT_FILENO);
                    close(stdout_link[0]);
                    close(stdout_link[1]);

                    dup2(stderr_link[1], STDERR_FILENO);
                    close(stderr_link[0]);
                    close(stderr_link[1]);

                    // pipe file into stdin
                    if (!input.empty())
                    {
                        const int fdin = open(input.c_str(), O_RDONLY);
                        if (fdin == -1)
                        {
                            cerr << "Failed to open fdin " << input << ": " << strerror(errno) << endl;
                        }
                        else
                        {
                            dup2(fdin, STDIN_FILENO);
                            close(fdin);
                        }
                    }

                    if (execv(path.c_str(), arguments) == -1)
                    {
                        cerr << strerror(errno) << endl;
                        perror("execl");
                        exit(excl_fail);
                    }
                    return;
                }
            default:
                cout << "Child process id from parent: " << pid << endl;

                close(stdout_link[1]);
                close(stderr_link[1]);
                const auto t1 = new thread(&read_pipe, stdout_link[0], ref(stdout));
                const auto t2 = new thread(&read_pipe, stderr_link[0], ref(stderr));
            }
        }

        void read_pipe(int link, string& out)
        {
            const int buf_size = 4096;
            char buffer[buf_size];
            out.clear();

            do
            {
                errno = 0;
                const ssize_t r = read(link, buffer, buf_size);
                if (r > 0)
                {
                    out.append(buffer, r);
                }
            }
            while (errno == EAGAIN || errno == EINTR);
        }

        bool is_child_running(const pid_t pid, int& status)
        {
            const pid_t result = waitpid(pid, &status, WNOHANG);
            return result == 0;
        }
    }
}
