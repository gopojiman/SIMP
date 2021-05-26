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
    private:
        ArrP arr;

    public:
        const int length;
        // Integer Constructor
        Value(int val):
            length(-1),arr(ArrP(new vector<int>(1, val))) {};
        // Array Constructor
        Value(int length, int val):
            length(length),arr(ArrP(new vector<int>(length, val))) {};
        // Returns integer value
        int val() const;
        // Returns value at array index pos, 0 if pos is invalid,
        // or val() if length < 0
        int at(int pos) const;
        // Puts value at array index pos, does nothing if pos is invalid
        void put(int pos, int val);
};

// Value smart pointer
typedef shared_ptr<Value> ValueP;
ostream& operator << (ostream& os, const ValueP& valueP);

class Store {
    private:
        inline static ValueP defaultValue = ValueP(new Value(0));
        map<string, ValueP> varMap;
    
    public:
        map<string, int> loopVarMap;
        void put(const string& key, ValueP valueP);
        ValueP& get(const string& key);
    
    friend ostream& operator << (ostream& os, const Store& store);
};

#endif