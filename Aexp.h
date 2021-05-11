#ifndef AEXP_H
#define AEXP_H

#include <string>
#include "Store.h"

class Aexp {
    public:
        virtual int eval(Store& store) const = 0;
        virtual ~Aexp() = default;
};

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
        const Aexp *left;
        const Aexp *right;
    
    public:
        AddExpr(Aexp *left, Aexp *right):left(left),right(right) {};
        int eval(Store& store) const;
        ~AddExpr();
};

class SubExpr: public Aexp {
    private:
        const Aexp *left;
        const Aexp *right;
    
    public:
        SubExpr(Aexp *left, Aexp *right):left(left),right(right) {};
        int eval(Store& store) const;
        ~SubExpr();
};

class MultExpr: public Aexp {
    private:
        const Aexp *left;
        const Aexp *right;
    
    public:
        MultExpr(Aexp *left, Aexp *right):left(left),right(right) {};
        int eval(Store& store) const;
        ~MultExpr();
};

#endif