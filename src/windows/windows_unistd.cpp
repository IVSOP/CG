#include "windows_unistd.h"

#include <chrono>
#include <thread>

void usleep(double usec) {
    std::this_thread::sleep_for(std::chrono::microseconds(static_cast<long long>(usec)));
}
