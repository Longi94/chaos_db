#pragma once

#include "cxxopts.hpp"
#include "flipper.hpp"

namespace chaos
{
    namespace args
    {
        /**
         * Check whether required option are present. Throws an exception if there is a required option missing.
         * @param args parsed option from cxxopts
         * @param required list of option names
         */
        void check_required(cxxopts::ParseResult args, const std::vector<std::string>& required);

        /**
         * Get the fault type from the arguments.
         * @param args parsed option from cxxopts
         * @return fault_type variable
         */
        flipper::fault_type get_fault_type(cxxopts::ParseResult args);
    }
}
