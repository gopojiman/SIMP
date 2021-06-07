#ifndef UTIL_H
#define UTIL_H

#include <thread>
#include <atomic>
#include "concurrentqueue.h"
using namespace std;

class Task;
typedef shared_ptr<Task> TaskP;

// class for static utility variables
class Util {
    public:
        inline static bool fullPrint = false;
        inline static int  n_threads = std::thread::hardware_concurrency();
        inline static atomic_int finished_threads = 0;
        inline static moodycamel::ConcurrentQueue<TaskP> workQueue;
};

#endif