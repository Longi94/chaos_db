#include "main.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <limits>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

int main()
{
    for (int i = 0; i < numeric_limits<int>::max(); ++i)
    {
        cout << i << endl;
        sleep_for(milliseconds(1000));
    }
    return 0;
}
