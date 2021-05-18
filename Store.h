#ifndef STORE_H
#define STORE_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

// Array smart pointer
typedef unique_ptr<vector<int>> ArrP;

// Value represents either an integer or an array of ints.
// length less than 0 means it's an integer, 
// which is stored in the first index of the vector
class Value {
    public:
        int length;
        ArrP arr;
        Value(int length, ArrP& arr):length(length),arr(move(arr)) {};
};

// Value smart pointer
typedef unique_ptr<Value> ValueP;
ostream& operator << (ostream& os, const ValueP& valueP);

class Store {
    private:
        inline static ArrP defaultArr = ArrP(new vector<int>(1, 0));
        inline static ValueP defaultValue = ValueP(new Value(-1, defaultArr));
        map<string, ValueP> varMap;
    
    public:
        void putNum(const string& key, int val);
        ValueP& get(const string& key);
    
    friend ostream& operator << (ostream& os, const Store& store);
};

#endif