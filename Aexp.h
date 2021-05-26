#ifndef AEXP_H
#define AEXP_H

#include <memory>
#include <functional>
#include "Store.h"

// Alias for Binary Aexp Function
typedef function<int(int, int)> BinAexpFunc;

class Aexp {
    public:
        // mapping from lowest precedence binary operators to functions
        static map<string, BinAexpFunc> binaryFuncs0;
        // mapping from highest precedence binary operators to functions
        static map<string, BinAexpFunc> binaryFuncs1;
        virtual ValueP eval(Store& store, int tid) const = 0;
        virtual ~Aexp() = default;
};

// Alias for Aexp smart pointer
typedef unique_ptr<const Aexp> AexpP;

class ValueAexp: public Aexp {
    private:
        const int length;
        const int val;

    public:
        ValueAexp(int length, int val):
            length(length),val(val) {};
        ValueP eval(Store& store, int tid) const;
};

class Var: public Aexp {
    private:
        const string name;

    public:
        Var(string name):name(name) {};
        ValueP eval(Store& store, int tid) const;
};

class BinaryAexp: public Aexp {
    private:
        const BinAexpFunc func;
        const AexpP left;
        const AexpP right;
    
    public:
        BinaryAexp(BinAexpFunc& func, AexpP& left, AexpP& right):
            func(func),left(move(left)),right(move(right)) {};
        ValueP eval(Store& store, int tid) const;
};

// Array reference with integer index, e.g. a[5]
class ArrayNumRef: public Aexp {
    private:
        const string name;
        const int index;
    
    public:
        ArrayNumRef(string name, int index):name(name),index(index) {};
        ValueP eval(Store& store, int tid) const;
};

#endif