#pragma once
#include <unistd.h>
#include <string>

constexpr auto pipe_read = 0;
constexpr auto pipe_write = 1;

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
        int attach(pid_t pid);

        /**
         * Execute a command by creating a child process with fork().
         * @param path path to the binary
         * @param output file to write the stdout of the child process to
         * @param input file to pipe into the stdin of the child process
         * @param arguments command line arguments for the child process
         * @return the process id of the child process
         */
        pid_t execute(std::string path, std::string output, std::string input, char** arguments);

        /**
         * Wait for a process (using waitpid) and print out information about the process.
         * @param pid process id
         */
        void wait_exit_code(pid_t pid);
    }
}
