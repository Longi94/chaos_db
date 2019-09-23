#include "flipper.hpp"
#include "cxxopts.hpp"
#include <random>

using namespace std;

int main(int argc, char* argv[])
{
    cxxopts::Options options("injector", "Inject bit flips into a child process.");
    options
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
        ("a,address", "The address to inject a bit flip into", cxxopts::value<long>())
        ("p,pid", "The child process id", cxxopts::value<int>());

    auto args = options.parse(argc, argv);

    if (args.count("help"))
    {
        cout << options.help({""}) << endl;
        exit(0);
    }

    if (args.count("pid") == 0)
    {
        cout << "Missing pid argument." << endl;
        exit(1);
    }

    // Init random
    srand(time(nullptr));

    const int pid = args["pid"].as<int>();

    long offset = -1;
    if (args.count("address"))
    {
        offset = args["address"].as<long>();
    }

    chaos::flipper::flip_random_bit(pid, offset);

    return 0;
}
