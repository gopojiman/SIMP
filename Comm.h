#ifndef COMM_H
#define COMM_H

#include "Bexp.h"

class Comm {
    public:
        virtual void eval(Store& store) = 0;
        virtual ~Comm() = default;
};

class SkipComm: public Comm {
    public:
        void eval(Store& store);
};

class AssignComm: public Comm {
    private:
        const string varName;
        AexpP aexpr;

    public:
        AssignComm(string varName, AexpP& aexpr):
            varName(varName),aexpr(move(aexpr)) {};
        void eval(Store& store);
};

class SeqComm: public Comm {
    private:
        Comm *left;
        Comm *right;

    public:
        SeqComm(Comm *left, Comm *right);
        void eval(Store& store);
        ~SeqComm();
};

class IfComm: public Comm {
    private:
        BexpP cond;
        Comm *trueComm;
        Comm *falseComm;

    public:
        IfComm(BexpP& cond, Comm *trueComm, Comm *falseComm):
            cond(move(cond)),trueComm(trueComm),falseComm(falseComm) {};
        void eval(Store& store);
        ~IfComm();
};

class WhileComm: public Comm {
    private:
        BexpP cond;
        Comm *body;

    public:
        WhileComm(BexpP& cond, Comm *body):
            cond(move(cond)),body(body) {};
        void eval(Store& store);
        ~WhileComm();
};

#endif