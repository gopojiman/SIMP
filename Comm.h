#ifndef COMM_H
#define COMM_H

#include <string>
#include "Store.h"
#include "Aexp.h"
#include "Bexp.h"

class Comm {
    public:
        virtual Comm *eval(Store *store) = 0;
        virtual Comm *copy() = 0;
        virtual string show() = 0;
        // delete is shallow for some Comms, obliterate is deep deletion
        virtual void obliterate();
        virtual ~Comm() = default;
};

class SkipComm: public Comm {
    public:
        Comm *eval(Store *store);
        Comm *copy();
        string show();
};

class AssignComm: public Comm {
    private:
        string varName;
        Aexp *aexpr;

    public:
        AssignComm(string varName, Aexp *aexpr);
        Comm *eval(Store *store);
        Comm *copy();
        string show();
        ~AssignComm();
};

class SeqComm: public Comm {
    private:
        Comm *left;
        Comm *right;

    public:
        SeqComm(Comm *left, Comm *right);
        Comm *eval(Store *store);
        Comm *copy();
        string show();
        void obliterate();
};

class IfComm: public Comm {
    private:
        Bexp *cond;
        Comm *trueComm;
        Comm *falseComm;

    public:
        IfComm(Bexp *cond, Comm *trueComm, Comm *falseComm);
        Comm *eval(Store *store);
        Comm *copy();
        string show();
        void obliterate();
};

class WhileComm: public Comm {
    private:
        Bexp *cond;
        Comm *body;

    public:
        WhileComm(Bexp *cond, Comm *body);
        Comm *eval(Store *store);
        Comm *copy();
        string show();
        ~WhileComm();
};

#endif