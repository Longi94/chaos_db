#include "args.hpp"
#include "fault.hpp"
#include "process.hpp"
#include "cxxopts.hpp"
#include "server.hpp"
#include "database.hpp"
#include "time.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <sys/wait.h>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <netdb.h>

using namespace std;
using namespace chaos;

int main(const int argc, char* argv[])
{
    cxxopts::Options options(
        "injector",
        "Inject bit flips into a child process. Example usage: injector -d results.sqlite -i queries/sqlite/1.sql -m 5000 -c /usr/bin/sqlite3 tpc-h.sqlite"
    );
    options
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
        ("c,command",
         "The command the child process should run. This should be the last option as anything after this will be interpreted as the command.",
         cxxopts::value<string>())
        ("d,database", "string: The SQLite database to save the result into.", cxxopts::value<string>())
        ("a,iteration", "Iteration number", cxxopts::value<int>())
        ("b,hostname", "Host machine name", cxxopts::value<string>())
        ("i,input", "string: File to pipe into stdin of the child process", cxxopts::value<string>()->default_value(""))
        ("f,fault", "string: The type of fault to inject. Can be \"flip\", \"stuck\".", cxxopts::value<string>())
        ("single", "flag: inject one single fault")
        ("m,mean-runtime", "long: The mean runtime of the experiment in milliseconds.", cxxopts::value<long>())
        ("flip-rate",
         "double: Frequency of bit-flips in a bit/second/megabytes unit. Required if the fault type is \"flip\".",
         cxxopts::value<double>())
        ("random-flip-rate",
         "flag: Randomize the frequency of bit flips keeping the flip-rate in mind. At least one flip is ensured if --mean-runtime is provided.")
        ("stuck-rate",
         "double: Frequency of stuck bits in a bit/megabytes unit. Required if the fault type is \"stuck\".",
         cxxopts::value<double>())
        ("x,heap", "flag: Inject faults into the heap.")
        ("y,anon", "flag: Inject faults into writable anonymous areas.")
        ("z,stack", "flag: Inject faults into the stack.")
        ("p,port",
         "integer: Enable inter-process communication using this port. The injector will wait for a message to start and stop the injection.",
         cxxopts::value<int>());

    // cxxopts modifies argc and argv (removing parsed arguments) so we make a copy to make it easier to manually parse later
    int argc_copy = argc;
    auto argv_copy = new char*[argc + 1];
    for (int i = 0; i <= argc; i++)
    {
        argv_copy[i] = argv[i];
    }

    auto args = options.parse(argc_copy, argv_copy);

    if (args.count("help"))
    {
        cout << options.help({""}) << endl;
        exit(0);
    }

    args::check_required(args, {"command", "database", "iteration", "hostname"});
    args::check_depend(args, "fault", "stuck", "stuck-rate");

    // Init random
    random_device dev;
    mt19937 rng(dev());

    auto path = args["command"].as<string>();
    auto input = args["input"].as<string>();
    auto database_name = args["database"].as<string>();
    auto hostname = args["hostname"].as<string>();
    auto iteration = 0;

    if (args.count("iteration"))
    {
        iteration = args["iteration"].as<int>();
    }

    const auto fault_type = args::get_fault_type(args);
    const auto command_args = args::get_command_arguments(argc, argv);

    const auto injector = get_injector(fault_type, args, rng);

    pid_t pid = 0;
    string stdout;
    string stderr;

    process::execute(path, input, command_args, stdout, stderr, pid);
    unique_ptr<fault::result> result(new fault::result());
    result->iteration = iteration;
    result->hostname = hostname.c_str();

    atomic_bool stop_flag(false);
    atomic_bool start_flag(false);
    unique_ptr<thread> server_thread = nullptr;
    int sock_fd = 0;
    if (args.count("port"))
    {
        mutex m;
        condition_variable cv;
        unique_lock<mutex> lk(m);
        server_thread = server::start_background(args["port"].as<int>(), m, cv, start_flag, stop_flag, sock_fd);

        cout << "Waiting for start command..." << endl;
        while (!start_flag)
        {
            cv.wait(lk);
        }
    }

    cout << "Injecting fault..." << endl;
    const auto start_ts = time::current_time_millis();
    injector->inject(pid, stop_flag);

    if (!stop_flag)
    {
        stop_flag = true;
        if (sock_fd > 0)
        {
            if (shutdown(sock_fd, SHUT_RDWR))
            {
                cout << "failed to shutdown server: " << strerror(errno) << endl;
            }
        }
    }

    injector->get_result(result);
    const auto runtime = time::current_time_millis() - start_ts;
    result->runtime = runtime;

    if (server_thread != nullptr)
    {
        server_thread->join();
    }

    result->stdout = stdout.c_str();
    result->stderr = stderr.c_str();
    database::save_result(database_name, result);

    return 0;
}
