#include "flipper.hpp"
#include <iostream>

using namespace std;

int main(const int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Usage: injector [pid]" << endl;
        return 0;
    }

    const int pid = strtol(argv[1], nullptr, 0);

    cout << "Got pid: " << argv[1] << endl;

    chaos::flipper::flip_random_bit(pid);

    return 0;
}
