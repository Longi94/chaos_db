#pragma once

#include "cxxopts.hpp"
#include "fault.hpp"
#include "memory.hpp"

namespace chaos
{
    namespace args
    {
        /**
         * Check whether required option are present. Throws an exception if there is a required option missing.
         * @param args parsed option from cxxopts
         * @param required list of option names
         */
        void check_required(cxxopts::ParseResult& args, const std::vector<std::string>& required);

        /**
         * If parent is present, child must be present too. Throws an exception of this dependency is violated.
         * @param args parsed option from cxxopts
         * @param parent option that depends on the child
         * @param value expected value of the parent option
         * @param child option that must be present if parent is present.
         */
        void check_depend(cxxopts::ParseResult& args, const std::string& parent, const std::string& value, const std::string& child);

        /**
         * Get the command line arguments for the child process from the parsed options.
         * @param argc argument count
         * @param argv arguments array
         */
        char** get_command_arguments(int argc, char* argv[]);

        /**
         * Get the fault type from the arguments.
         * @param args parsed option from cxxopts
         * @return fault_type variable
         */
        fault::fault_type get_fault_type(cxxopts::ParseResult& args);
    }
}
