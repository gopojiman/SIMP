#ifndef UTIL_H
#define UTIL_H

#include <thread>
#include <atomic>
#include "concurrentqueue.h"
using namespace std;

class Store;
class Comm;
class Task;
typedef shared_ptr<const Comm> CommP;
typedef shared_ptr<Task> TaskP;

// Wrapper class for Comm that sets an atomic boolean flag
// and decrements its children's num_parents value after it's evaluated. 
// Automatically enqueues children to the workQueue if num_parents is
// decreased to 0.
class Task {
    private:    
        atomic_bool done;
        atomic_int num_parents;
    
    public:
        const CommP comm; 
        list<TaskP> children;
        Task(CommP comm):
            comm(comm),done(false),num_parents(0) {};
        bool isDone() {return done.load();};
        void eval(Store& store, int tid);
        int decrement_parents();
        void init_parents(int n_parents) {num_parents.store(n_parents);};
};

// class for static utility variables
class Util {
    public:
        inline static bool fullPrint = false;
        inline static int  n_threads = std::thread::hardware_concurrency();
        inline static atomic_int finished_threads = 0;
        inline static moodycamel::ConcurrentQueue<TaskP> workQueue;
};

#endif