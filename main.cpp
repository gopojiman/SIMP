#include <fstream>
#include "Parser.h"
#include "Util.h"

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

    comm->eval(store);
    cout << store << endl;
}