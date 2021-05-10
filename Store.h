#ifndef STORE_H
#define STORE_H

#include <iostream>
#include <string>
#include <map>
using namespace std;

class Store {
    private:
        map<string, int> storeMap;
    
    public:
        void put(string key, int val);
        int get(string key);
    
    friend ostream& operator << (ostream& os, const Store &store);
};

#endif