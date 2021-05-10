#ifndef AEXP_H
#define AEXP_H

#include <string>
#include "Store.h"

class Aexp {
    public:
        virtual int eval(Store *store) = 0;
        virtual Aexp *copy() = 0;
        virtual string show() = 0;
        virtual ~Aexp() = default;
    
    friend ostream& operator << (ostream& os, const Store &store);
};

class NumExpr: public Aexp {
    private:
        int val;

    public:
        NumExpr(int val);
        int eval(Store *store);
        Aexp *copy();
        string show();
};

class VarExpr: public Aexp {
    private:
        string name;

    public:
        VarExpr(string name);
        int eval(Store *store);
        Aexp *copy();
        string show();
};

class AddExpr: public Aexp {
    private:
        Aexp *left;
        Aexp *right;
    
    public:
        AddExpr(Aexp *left, Aexp *right);
        int eval(Store *store);
        Aexp *copy();
        string show();
        ~AddExpr();
};

class SubExpr: public Aexp {
    private:
        Aexp *left;
        Aexp *right;
    
    public:
        SubExpr(Aexp *left, Aexp *right);
        int eval(Store *store);
        Aexp *copy();
        string show();
        ~SubExpr();
};

class MultExpr: public Aexp {
    private:
        Aexp *left;
        Aexp *right;
    
    public:
        MultExpr(Aexp *left, Aexp *right);
        int eval(Store *store);
        Aexp *copy();
        string show();
        ~MultExpr();
};

#endif