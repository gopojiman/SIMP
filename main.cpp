#include <iostream>
#include <string>
#include "Store.h"
#include "Comm.h"
#include "Parser.h"
using namespace std;


int main() {
    Store *store = new Store();
    string inputStr;
    getline(cin, inputStr);

    Parser parser = Parser(inputStr);
    Comm* comm = parser.parseComm();
    comm = comm->eval(store);
    for (int i = 0; comm != NULL && i < 10000; i++) {
        cout << "⇒ " << comm->show() << ", " << *store << endl;
        comm = comm->eval(store);
    }
    delete store;
}