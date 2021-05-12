#ifndef BEXP_H
#define BEXP_H

#include "Aexp.h"

class Bexp {
    public:
        virtual bool eval(Store& store) const = 0;
        virtual ~Bexp() = default;
};

// Alias for Bexp smart pointer
typedef unique_ptr<const Bexp> BexpP;

class TrueExpr: public Bexp {
    public:
        bool eval(Store& store) const;
};

class FalseExpr: public Bexp {
    public:
        bool eval(Store& store) const;
};

class EqExpr: public Bexp {
    private:
        AexpP left;
        AexpP right;

    public:
        EqExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        bool eval(Store& store) const;
};

class LessExpr: public Bexp {
    private:
        AexpP left;
        AexpP right;

    public:
        LessExpr(AexpP& left, AexpP& right):
            left(move(left)),right(move(right)) {};
        bool eval(Store& store) const;
};

class NotExpr: public Bexp {
    private:
        BexpP oper;

    public:
        NotExpr(BexpP& oper):oper(move(oper)) {};
        bool eval(Store& store) const;
};

class AndExpr: public Bexp {
    private:
        BexpP left;
        BexpP right;

    public:
        AndExpr(BexpP& left, BexpP& right):
            left(move(left)),right(move(right)) {};
        bool eval(Store& store) const;
};

class OrExpr: public Bexp {
    private:
        BexpP left;
        BexpP right;

    public:
        OrExpr(BexpP& left, BexpP& right):
            left(move(left)),right(move(right)) {};
        bool eval(Store& store) const;
};

#endif