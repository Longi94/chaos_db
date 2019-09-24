#include "cxxopts.hpp"

namespace chaos
{
    namespace args
    {
        void check_required(const cxxopts::ParseResult args, const std::vector<std::string>& required)
        {
            for (auto& r : required)
            {
                if (args.count(r) == 0)
                {
                    throw cxxopts::option_required_exception(r);
                }
            }
        }
    }
}
