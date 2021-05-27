#ifndef UTIL_H
#define UTIL_H

#include <thread>

// class for static utility variables
class Util {
    public:
        inline static bool fullPrint = false;
        inline static int  n_threads = std::thread::hardware_concurrency();
};

#endif