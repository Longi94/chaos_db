#include "flipper.hpp"
#include <iostream>

using namespace std;

int main(const int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: injector [pid] [address]" << endl;
        return 0;
    }

    const int pid = strtol(argv[1], nullptr, 0);
    const long offset = strtol(argv[2], nullptr, 16);

    chaos::flipper::flip_random_bit(pid, offset);

    return 0;
}
