#include "Parser.h"

int main() {
    Store store;
    string inputStr;
    getline(cin, inputStr);

    Parser parser = Parser(inputStr);
    CommP comm = parser.parseComm();

    comm->eval(store);
    cout << store << endl;
}