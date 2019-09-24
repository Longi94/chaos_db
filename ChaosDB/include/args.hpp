#pragma once

#include "cxxopts.hpp"

namespace chaos
{
    namespace args
    {
        void check_required(const cxxopts::ParseResult args, const std::vector<std::string>& required);
    }
}
