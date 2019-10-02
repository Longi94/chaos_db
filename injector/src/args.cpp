#include "cxxopts.hpp"
#include "flipper.hpp"
#include <string>

using namespace std;

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

        flipper::fault_type get_fault_type(const cxxopts::ParseResult args)
        {
            if (!args.count("fault"))
            {
                return flipper::flip;
            }

            const auto fault_str = args["fault"].as<string>();

            if (fault_str.compare("flip") == 0)
            {
                return flipper::flip;
            }
            else if (fault_str.compare("stuck") == 0)
            {
                return flipper::stuck;
            }

            return flipper::flip;
        }
    }
}
