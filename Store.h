#ifndef STORE_H
#define STORE_H

#include <iostream>
#include <string>
#include <map>
using namespace std;

class Store {
    private:
        map<string, int> varMap;
    
    public:
        void putVar(string key, int val);
        int getVar(string key);
    
    friend ostream& operator << (ostream& os, const Store &store);
};

#endif