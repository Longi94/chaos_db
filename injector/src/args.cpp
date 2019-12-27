#include "args.hpp"
#include "cxxopts.hpp"
#include "fault.hpp"
#include "memory.hpp"

using namespace std;

namespace chaos
{
    namespace args
    {
        void check_required(cxxopts::ParseResult& args, const vector<string>& required)
        {
            for (auto& r : required)
            {
                if (args.count(r) == 0)
                {
                    throw cxxopts::option_required_exception(r);
                }
            }
        }

        void check_depend(cxxopts::ParseResult& args, const string& parent, const string& value, const string& child)
        {
            if (args.count(parent) > 0 && args[parent].as<string>().compare(value) == 0 && args.count(child) == 0)
            {
                throw cxxopts::option_required_exception(child);
            }
        }

        char** get_command_arguments(const int argc, char* argv[])
        {
            // Find where -c or --command is to find the arguments after that, those will be passed to the child process
            const auto arg_end = argv + argc;
            char** command_option = find(argv, arg_end, string("-c"));
            if (command_option == arg_end)
            {
                command_option = find(argv, arg_end, string("--command"));
            }
            return command_option + 1;
        }

        fault::fault_type get_fault_type(cxxopts::ParseResult& args)
        {
            if (!args.count("fault"))
            {
                return fault::none;
            }

            const auto fault_str = args["fault"].as<string>();

            if (fault_str.compare("flip") == 0)
            {
                return fault::flip;
            }
            if (fault_str.compare("stuck") == 0)
            {
                return fault::stuck;
            }

            return fault::none;
        }
    }
}
