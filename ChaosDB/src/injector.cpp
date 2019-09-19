#include "flipper.hpp"
#include <iostream>
#include <random>

using namespace std;

int main(const int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: injector pid [address]" << endl;
        return 0;
    }

    // Init random
    srand(time(nullptr));

    const int pid = strtol(argv[1], nullptr, 0);

    long offset = -1;
    if (argc >= 3)
    {
        offset = strtol(argv[2], nullptr, 16);
    }

    chaos::flipper::flip_random_bit(pid, offset);

    return 0;
}
