#ifndef BEXP_H
#define BEXP_H

#include "Aexp.h"

class Bexp {
    public:
        virtual bool eval(Store& store) = 0;
        virtual ~Bexp() = default;
};

class TrueExpr: public Bexp {
    public:
        bool eval(Store& store);
};

class FalseExpr: public Bexp {
    public:
        bool eval(Store& store);
};

class EqExpr: public Bexp {
    private:
        AexpP left;
        AexpP right;

    public:
        EqExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        bool eval(Store& store);
};

class LessExpr: public Bexp {
    private:
        AexpP left;
        AexpP right;

    public:
        LessExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        bool eval(Store& store);
};

class NotExpr: public Bexp {
    private:
        Bexp *oper;

    public:
        NotExpr(Bexp *oper);
        bool eval(Store& store);
        ~NotExpr();
};

class AndExpr: public Bexp {
    private:
        Bexp *left;
        Bexp *right;

    public:
        AndExpr(Bexp *left, Bexp *right);
        bool eval(Store& store);
        ~AndExpr();
};

class OrExpr: public Bexp {
    private:
        Bexp *left;
        Bexp *right;

    public:
        OrExpr(Bexp *left, Bexp *right);
        bool eval(Store& store);
        ~OrExpr();
};

#endif