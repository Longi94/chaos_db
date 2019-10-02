#include "args.hpp"
#include "flipper.hpp"
#include "memory.hpp"
#include "process.hpp"
#include "cxxopts.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <sys/wait.h>
#include <chrono>
#include <thread>

using namespace std;
using namespace chaos;

int main(const int argc, char* argv[])
{
    cxxopts::Options options("injector", "Inject bit flips into a child process. Example usage: injector -o q1.out -i queries/sqlite/1.sql -m 5000 -c /usr/bin/sqlite3 tpc-h.sqlite");
    options
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
        ("c,command",
         "The command the child process should run. This should be the last option as anything after this will be interpreted as the command.",
         cxxopts::value<string>())
        ("o,output", "Out put file to save the output of the command to", cxxopts::value<string>())
        ("i,input", "File to pipe into stdin of the child process", cxxopts::value<string>()->default_value(""))
        ("f,fault", "The type of fault to inject. Can be \"flip\", \"stuck\".", cxxopts::value<string>())
        ("d,delay", "Milliseconds to wait before injecting a bit flip into the child process", cxxopts::value<unsigned long>())
        ("s,inject-space", "Address space to inject the fault into. Can be \"heap\" or \"stack\". If not provided it will be randomly chosen",
                 cxxopts::value<string>());

    // cxxopts modifies argc and argv (removing parsed arguments) so we make a copy to make it easier to manually parse later
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
    const auto input = args["input"].as<string>();

    const auto space = args::get_memory_space(args);
    const auto fault_type = args::get_fault_type(args);
    const auto command_args = args::get_command_arguments(argc, argv);

    const pid_t pid = process::execute(path, output, input, command_args);

    if (fault_type == flipper::none)
    {
        const auto ms = args["delay"].as<unsigned long>();
        cout << "Flip random bit in " << ms << " milliseconds..." << endl;
        this_thread::sleep_for(chrono::milliseconds(ms));
        flipper::flip_random_bit(pid, -1, space);
    }

    process::wait_exit_code(pid);

    return 0;
}
