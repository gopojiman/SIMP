#ifndef COMM_H
#define COMM_H

#include "Bexp.h"

class Comm {
    public:
        virtual void eval(Store& store) const = 0;
        virtual ~Comm() = default;
};

// Alias for Comm smart pointer
typedef unique_ptr<const Comm> CommP;

class SkipComm: public Comm {
    public:
        void eval(Store& store) const;
};

class AssignComm: public Comm {
    private:
        const string varName;
        AexpP aexpr;

    public:
        AssignComm(string varName, AexpP& aexpr):
            varName(varName),aexpr(move(aexpr)) {};
        void eval(Store& store) const;
};

class SeqComm: public Comm {
    private:
        CommP left;
        CommP right;

    public:
        SeqComm(CommP& left, CommP& right):
            left(move(left)),right(move(right)) {};
        void eval(Store& store) const;
};

class IfComm: public Comm {
    private:
        BexpP cond;
        CommP trueComm;
        CommP falseComm;

    public:
        IfComm(BexpP& cond, CommP& trueComm, CommP& falseComm):
            cond(move(cond)),
            trueComm(move(trueComm)),
            falseComm(move(falseComm)) {};
        void eval(Store& store) const;
};

class WhileComm: public Comm {
    private:
        BexpP cond;
        CommP body;

    public:
        WhileComm(BexpP& cond, CommP& body):
            cond(move(cond)),body(move(body)) {};
        void eval(Store& store) const;
};

#endif