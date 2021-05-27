#include <fstream>
#include <atomic>
#include "Parser.h"
#include "Util.h"

void thread_func(int tid, atomic_int *finished_threads, Store *store, CQ *workQueue) {
    TaskP task;
    bool finished = false;
    while (finished_threads->load() < Util::n_threads) {
        if (workQueue->try_dequeue(task)) {
            if (finished) {
                finished = false;
                atomic_fetch_add(finished_threads, -1);
            }
            task->eval(*store, tid, *workQueue);
        } else if (!finished) {
            finished = true;
            atomic_fetch_add(finished_threads, 1);
        }
    }
}

int main(int argc, char** argv) {
    Store store;
    Parser *parser;
    char *arg;
    
    if (argc == 1) {
        parser = new Parser(cin);
    }
    else if (argc > 3) {
        cerr << "Usage: " << argv[0] << " [--full-print] [inputFilename]" << endl;
        exit(1);
    }
    else {
        arg = argv[1];
        if (strcmp(arg, "--full-print") == 0) {
            Util::fullPrint = true;
            if (argc > 2) {
                arg = argv[2];
            }
            else {
                arg = NULL;
            }
        }
        if (arg != NULL && strcmp(arg, "-") != 0) {
            ifstream inputStream(arg);
            if (!inputStream.is_open()) {
                cerr << "Error: Unable to open input file" << endl;
                exit(1);
            }
            parser = new Parser(inputStream);
        }
        else {
            parser = new Parser(cin);
        }
    }

    CommP comm = parser->parseComm();
    delete parser;
    CQ workQueue;

#ifdef NOPARALLEL
    comm->eval(store, 0, workQueue);
#else
    atomic_int finished_threads(0);
    thread threads[Util::n_threads];

    TaskP task(new Task(comm));
    workQueue.enqueue(task);

    for (int i = 0; i < Util::n_threads; i++) {
        threads[i] = thread(thread_func, i, &finished_threads, &store, &workQueue);
    }
    for (int i = 0; i < Util::n_threads; i++) {
        threads[i].join();
    }
#endif
    cout << store << endl;
}