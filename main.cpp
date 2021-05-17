#include <fstream>
#include "Parser.h"

int main(int argc, char** argv) {
    Store store;
    Parser *parser;
    
    if (argc == 2 && strcmp(argv[1], "-") != 0) {
        ifstream inputStream(argv[1]);
        if (!inputStream.is_open()) {
            cerr << "Error: Unable to open input file" << endl;
            exit(1);
        }
        parser = new Parser(inputStream);
    }
    else if (argc > 2) {
        cerr << "Usage: " << argv[0] << " [filename]" << endl;
        exit(1);
    }
    else {
        parser = new Parser(cin);
    }
    CommP comm = parser->parseComm();
    delete parser;

    comm->eval(store);
    cout << store << endl;
}