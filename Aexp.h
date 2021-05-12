#ifndef AEXP_H
#define AEXP_H

#include <memory>
#include "Store.h"

class Aexp {
    public:
        virtual int eval(Store& store) const = 0;
        virtual ~Aexp() = default;
};

// Alias for Aexp smart pointer
typedef unique_ptr<const Aexp> AexpP;

class NumExpr: public Aexp {
    private:
        const int val;

    public:
        NumExpr(int val):val(val) {};
        int eval(Store& store) const;
};

class VarExpr: public Aexp {
    private:
        const string name;

    public:
        VarExpr(string name):name(name) {};
        int eval(Store& store) const;
};

class AddExpr: public Aexp {
    private:
        AexpP left;
        AexpP right;
    
    public:
        AddExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

class SubExpr: public Aexp {
    private:
        AexpP left;
        AexpP right;
    
    public:
        SubExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

class MultExpr: public Aexp {
    private:
        AexpP left;
        AexpP right;
    
    public:
        MultExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

#endif