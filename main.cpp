#include <iostream>
#include <string>
#include "Store.h"
#include "Comm.h"
#include "Parser.h"
using namespace std;


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