#ifndef BEXP_H
#define BEXP_H

#include <string>
#include "Store.h"
#include "Aexp.h"

class Bexp {
    public:
        virtual bool eval(Store *store) = 0;
        virtual Bexp *copy() = 0;
        virtual string show() = 0;
        virtual ~Bexp() = default;
};

class TrueExpr: public Bexp {
    public:
        bool eval(Store *store);
        Bexp *copy();
        string show();
};

class FalseExpr: public Bexp {
    public:
        bool eval(Store *store);
        Bexp *copy();
        string show();
};

class EqExpr: public Bexp {
    private:
        Aexp *left;
        Aexp *right;

    public:
        EqExpr(Aexp *left, Aexp *right);
        bool eval(Store *store);
        Bexp *copy();
        string show();
        ~EqExpr();
};

class LessExpr: public Bexp {
    private:
        Aexp *left;
        Aexp *right;

    public:
        LessExpr(Aexp *left, Aexp *right);
        bool eval(Store *store);
        Bexp *copy();
        string show();
        ~LessExpr();
};

class NotExpr: public Bexp {
    private:
        Bexp *oper;

    public:
        NotExpr(Bexp *oper);
        bool eval(Store *store);
        Bexp *copy();
        string show();
        ~NotExpr();
};

class AndExpr: public Bexp {
    private:
        Bexp *left;
        Bexp *right;

    public:
        AndExpr(Bexp *left, Bexp *right);
        bool eval(Store *store);
        Bexp *copy();
        string show();
        ~AndExpr();
};

class OrExpr: public Bexp {
    private:
        Bexp *left;
        Bexp *right;

    public:
        OrExpr(Bexp *left, Bexp *right);
        bool eval(Store *store);
        Bexp *copy();
        string show();
        ~OrExpr();
};

#endif