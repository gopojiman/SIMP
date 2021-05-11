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
        unique_ptr<const Aexp> aexpr;

    public:
        AssignComm(string varName, unique_ptr<const Aexp>& aexpr):
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
        Bexp *cond;
        Comm *trueComm;
        Comm *falseComm;

    public:
        IfComm(Bexp *cond, Comm *trueComm, Comm *falseComm);
        void eval(Store& store);
        ~IfComm();
};

class WhileComm: public Comm {
    private:
        Bexp *cond;
        Comm *body;

    public:
        WhileComm(Bexp *cond, Comm *body);
        void eval(Store& store);
        ~WhileComm();
};

#endif