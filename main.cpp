#include "Parser.h"

int main() {
    Store store;
    string inputStr;
    getline(cin, inputStr);

    Parser parser = Parser(inputStr);
    Comm* comm = parser.parseComm();

    comm->eval(store);
    cout << store << endl;
    delete comm;
}