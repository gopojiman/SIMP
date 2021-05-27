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
        // if the Aexp reads from a variable, adds it to the set
        virtual void readsFrom(VarSet& set) const = 0;
        virtual ~Aexp() = default;
};

// Alias for Aexp smart pointer
typedef unique_ptr<const Aexp> AexpP;

// Always represents an integer
class Num: public Aexp {
    private:
        const int val;
    
    public:
        Num(int val):val(val) {};
        ValueP eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const {};
};

// Represents an integer if length < 0, or an array otherwise
// length and val cannot be BinaryAexp in practice
class ValueAexp: public Aexp {
    private:
        const AexpP length;
        const AexpP val;

    public:
        ValueAexp(AexpP& length, AexpP& val):
            length(move(length)),val(move(val)) {};
        ValueP eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
};

class Var: public Aexp {
    private:
        const string name;

    public:
        Var(string name):name(name) {};
        ValueP eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
};

class LoopVar: public Aexp {
    private:
        const string name;

    public:
        LoopVar(string name):name(name) {};
        ValueP eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const {};
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
        void readsFrom(VarSet& set) const;
};

// Array reference with integer index, e.g. a[5]
class ArrayNumRef: public Aexp {
    private:
        const string name;
        const int index;
    
    public:
        ArrayNumRef(string name, int index):name(name),index(index) {};
        ValueP eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
};

// Array reference with a LoopVar, e.g. a[i]
class ArrayLoopRef: public Aexp {
    private:
        const string arrayName;
        const string loopVar;
    
    public:
        ArrayLoopRef(string arrayName, string loopVar):
            arrayName(arrayName),loopVar(loopVar) {};
        ValueP eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
};

#endif