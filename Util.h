#ifndef UTIL_H
#define UTIL_H

#include <thread>
#include "concurrentqueue.h"

class Aexp;
// Alias for Aexp smart pointer
typedef unique_ptr<const Aexp> AexpP;

class Bexp;
// Alias for Bexp smart pointer
typedef unique_ptr<const Bexp> BexpP;

class Comm;
// Alias for Comm smart pointer
typedef unique_ptr<const Comm> CommP;

#include "Comm.h"

typedef moodycamel::ConcurrentQueue<CommP> CQ;

// class for static utility variables
class Util {
    public:
        inline static bool fullPrint = false;
        inline static int  n_threads = std::thread::hardware_concurrency();
};

#endif