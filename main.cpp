#include <fstream>
#include <atomic>
#include "Parser.h"
#include "Util.h"

atomic_int finished_threads(0);

void thread_func(int tid) {
    cout << tid;
    atomic_fetch_add(&finished_threads, 1);
    cout << finished_threads.load();
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

#ifdef NOPARALLEL
    comm->eval(store, 0);
#else
    thread threads[Util::n_threads];
    for (int i = 0; i < Util::n_threads; i++) {
        threads[i] = thread(thread_func, i);
    }
    for (int i = 0; i < Util::n_threads; i++) {
        threads[i].join();
    }
#endif
    cout << store << endl;
}