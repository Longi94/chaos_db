#include "main.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <limits>

using namespace std;

int main()
{
    for (int i = 0; i < numeric_limits<int>::max(); ++i)
    {
        cout << i << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
    return 0;
}
