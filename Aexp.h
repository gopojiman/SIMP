#ifndef AEXP_H
#define AEXP_H

#include <memory>
#include <functional>
#include "Store.h"

// Alias for Binary Aexp Function
typedef function<int(int, int)> BinAexpFunc;

class Aexp {
    public:
        static map<string, BinAexpFunc> binaryFuncs; // mapping from binary operators to functions
        virtual int eval(Store& store) const = 0;
        virtual ~Aexp() = default;
};

// Alias for Aexp smart pointer
typedef unique_ptr<const Aexp> AexpP;

class Num: public Aexp {
    private:
        const int val;

    public:
        Num(int val):val(val) {};
        int eval(Store& store) const;
};

class Var: public Aexp {
    private:
        const string name;

    public:
        Var(string name):name(name) {};
        int eval(Store& store) const;
};

class BinaryAexp: public Aexp {
    private:
        BinAexpFunc func;
        AexpP left;
        AexpP right;
    
    public:
        BinaryAexp(BinAexpFunc& func, AexpP& left, AexpP& right):
            func(func),left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

#endif