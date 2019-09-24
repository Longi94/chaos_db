#pragma once

#include "cxxopts.hpp"

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
    }
}
