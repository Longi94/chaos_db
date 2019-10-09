#include "args.hpp"
#include "fault.hpp"
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
    cxxopts::Options options(
        "injector",
        "Inject bit flips into a child process. Example usage: injector -o q1.out -i queries/sqlite/1.sql -m 5000 -c /usr/bin/sqlite3 tpc-h.sqlite"
    );
    options
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
        ("c,command",
         "The command the child process should run. This should be the last option as anything after this will be interpreted as the command.",
         cxxopts::value<string>())
        ("o,output", "string: Redirect stdout of the child process into this file", cxxopts::value<string>())
        ("e,error", "string: Redirect stderr of the child process into this file", cxxopts::value<string>())
        ("i,input", "string: File to pipe into stdin of the child process", cxxopts::value<string>()->default_value(""))
        ("f,fault", "string: The type of fault to inject. Can be \"flip\", \"stuck\".", cxxopts::value<string>())
        ("m,mean-runtime", "long: The mean runtime of the experiment in milliseconds.", cxxopts::value<long>())
        ("flip-rate", "double: Frequency of bit-flips in a bit/second/megabytes unit. Required if the fault type is \"flip\".",
         cxxopts::value<double>())
        ("random-flip-rate", "flag: Randomize the frequency of bit flips keeping the flip-rate in mind. At least one flip is ensured if --mean-runtime is provided.")
        ("stuck-rate", "double: Frequency of stuck bits in a bit/megabytes unit. Required if the fault type is \"stuck\".",
         cxxopts::value<double>())
        ("s,inject-space",
         "string: Address space to inject the fault into. Can be \"heap\" or \"stack\". If not provided it will be randomly chosen",
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
    args::check_depend(args, "fault", "flip", "flip-rate");
    args::check_depend(args, "fault", "stuck", "stuck-rate");

    // Init random
    random_device dev;
    mt19937 rng(dev());

    auto path = args["command"].as<string>();
    auto output = args["output"].as<string>();
    auto input = args["input"].as<string>();
    string error = string();

    if (args.count("error"))
    {
        error = args["error"].as<string>();
    }

    const auto fault_type = args::get_fault_type(args);
    const auto command_args = args::get_command_arguments(argc, argv);

    const auto injector = get_injector(fault_type, args, rng);

    const pid_t pid = process::execute(path, output, input, error, command_args);

    atomic_bool running(true);
    thread inject_thread(&fault::FaultInjector::inject, injector.get(), pid, ref(running));

    cout << "Waiting for process to finish..." << endl;
    process::wait_exit_code(pid);

    running = false;
    inject_thread.join();

    injector->print_data();

    return 0;
}
