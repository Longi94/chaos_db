#include "time.hpp"
#include <chrono>

using namespace std;

long chaos::time::current_time_millis()
{
    return chrono::system_clock::now().time_since_epoch().count() / 1000000;
}
