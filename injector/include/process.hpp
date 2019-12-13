#pragma once
#include <unistd.h>
#include <string>

namespace chaos
{
    namespace process
    {
        /**
         * Attach to a process using ptrace. This blocks the thread.
         * @param pid process id
         * @return 0 if successful, -1 if there was an error
         */
        int detach(pid_t pid);

        /**
         * Detach from a process using ptrace.
         * @param pid process id
         * @return 0 if successful, -1 if there was an error
         */
        int attach(pid_t pid, int& status);

        /**
         * Execute a command by creating a child process with fork().
         * @param path path to the binary
         * @param input file to pipe into the stdin of the child process
         * @param arguments command line arguments for the child process
         * @return the process id of the child process
         */
        void execute(std::string& path, std::string& input, char** arguments, std::string& stdout, std::string& stderr, pid_t& pid);

        void read_pipe(int link, std::string& out);

        /**
         * Check if a child process is running.
         * @param pid process id
         * @param status
         */ 
        bool is_child_running(pid_t pid, int& status);
    }
}
