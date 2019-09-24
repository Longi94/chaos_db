#include "args.hpp"
#include "flipper.hpp"
#include "process.hpp"
#include "cxxopts.hpp"
#include <iostream>
#include <random>

using namespace std;
using namespace chaos;

int main(int argc, char* argv[])
{
    cxxopts::Options options("injector", "Inject bit flips into a child process.");
    options
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
        ("c,command",
         "The command the child process should run. This should be the last option as anything after this will be interpreted as the command.",
         cxxopts::value<string>())
        ("o,output", "Out put file to save the output of the command to", cxxopts::value<string>());

    int argc_copy = argc;
    char** argv_copy = argv;

    auto args = options.parse(argc_copy, argv_copy);

    if (args.count("help"))
    {
        cout << options.help({""}) << endl;
        exit(0);
    }

    args::check_required(args, {"command", "output"});

    // Init random
    srand(time(nullptr));

    const auto path = args["command"].as<string>();
    const auto output = args["output"].as<string>();

    const auto arg_end = argv + argc;
    char** command_option = find(argv, arg_end, string("-c"));

    if (command_option == arg_end)
    {
        command_option = find(argv, arg_end, string("--command"));
    }

    char** command_args = command_option + 1;

    process::execute(path, output, command_args);

    //flipper::flip_random_bit(pid, offset);

    return 0;
}
