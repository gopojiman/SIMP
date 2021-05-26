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
        const AexpP aexp;

    public:
        AssignComm(string varName, AexpP& aexp):
            varName(varName),aexp(move(aexp)) {};
        void eval(Store& store) const;
};

class AssignNumRefComm: public Comm {
    private:
        const string varName;
        const int index;
        const AexpP aexp;
    
    public:
        AssignNumRefComm(string varName, int index, AexpP& aexp):
            varName(varName),index(index),aexp(move(aexp)) {};
        void eval(Store& store) const;
};

class SeqComm: public Comm {
    private:
        const CommP left;
        const CommP right;

    public:
        SeqComm(CommP& left, CommP& right):
            left(move(left)),right(move(right)) {};
        void eval(Store& store) const;
};

class IfComm: public Comm {
    private:
        const BexpP cond;
        const CommP trueComm;
        const CommP falseComm;

    public:
        IfComm(BexpP& cond, CommP& trueComm, CommP& falseComm):
            cond(move(cond)),
            trueComm(move(trueComm)),
            falseComm(move(falseComm)) {};
        void eval(Store& store) const;
};

class WhileComm: public Comm {
    private:
        const BexpP cond;
        const CommP body;

    public:
        WhileComm(BexpP& cond, CommP& body):
            cond(move(cond)),body(move(body)) {};
        void eval(Store& store) const;
};

#endif