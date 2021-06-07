#ifndef COMM_H
#define COMM_H

#include "Bexp.h"

class Comm;
typedef shared_ptr<const Comm> CommP;

// Wrapper class for Comm that sets an atomic boolean flag
// and decrements its children's num_parents value after it's evaluated. 
// Automatically enqueues children to the workQueue if num_parents is
// decreased to 0.
class Task {
    private:    
        atomic_bool done;
        atomic_int num_parents;
    
    public:
        const CommP comm; 
        list<TaskP> children;
        Task(CommP comm):
            comm(comm),done(false),num_parents(0) {};
        bool isDone() {return done.load();};
        void eval(Store& store, int tid);
        int decrement_parents();
        void init_parents(int n_parents) {num_parents.store(n_parents);};
};

class Comm {
    public:
        virtual void eval(Store& store, int tid) const = 0;
        virtual void readsFrom(VarSet& set) const = 0;
        virtual void writesTo(VarSet& set) const = 0;
        virtual ~Comm() = default;
};

// true -> c2 depends on c1
bool notInterleavable(const CommP& c1, const CommP& c2);

// true -> body is safe to parallelize in a for loop
bool notParallelizableLoopBody(const CommP& body, const int start, const int end, const int step);


class SkipComm: public Comm {
    public:
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const {};
        void writesTo(VarSet& set) const {};
};

class AssignComm: public Comm {
    private:
        const string varName;
        const AexpP aexp;

    public:
        AssignComm(string varName, AexpP& aexp):
            varName(varName),aexp(move(aexp)) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

class AssignNumRefComm: public Comm {
    private:
        const string varName;
        const int index;
        const AexpP aexp;
    
    public:
        AssignNumRefComm(string varName, int index, AexpP& aexp):
            varName(varName),index(index),aexp(move(aexp)) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

class AssignLoopRefComm: public Comm {
    private:
        const string varName;
        const string loopVar;
        const AexpP aexp;
    
    public:
        AssignLoopRefComm(string varName, string loopVar, AexpP& aexp):
            varName(varName),loopVar(loopVar),aexp(move(aexp)) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

class SeqComm: public Comm {
    private:
        const list<CommP> comms;

    public:
        SeqComm(list<CommP> comms): comms(comms) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
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
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

class WhileComm: public Comm {
    private:
        const BexpP cond;
        const CommP body;

    public:
        WhileComm(BexpP& cond, CommP& body):
            cond(move(cond)),body(move(body)) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

class ForComm: public Comm {
    private:
        const string loopVarName;
        const AexpP start;
        const AexpP end;
        const AexpP step;
        const CommP body;
    
    public:
        ForComm(string loopVarName, AexpP& start, AexpP& end, AexpP& step, CommP& body):
            loopVarName(loopVarName),start(move(start)),end(move(end)),step(move(step)),body(move(body)) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

// All the below classes do not correspond to specific command types in
// the language, they are intermediary constructs used for parallelism

class PartialBinaryAexp: public Comm {
    private:
        const BinAexpFunc func;
        const ValueP leftEval;
        const ValueP rightEval;
        ValueP result;
        const int chunk_start;
        const int chunk_end;

    public:
        PartialBinaryAexp(const BinAexpFunc& func, const ValueP& leftEval, const ValueP& rightEval, ValueP& result, const int chunk_start, const int chunk_end):
            func(func),leftEval(leftEval),rightEval(rightEval),result(result),chunk_start(chunk_start),chunk_end(chunk_end) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const {};
        void writesTo(VarSet& set) const {};

};

class PartialCompareBexp: public Comm {
    private:
        const CompBexpFunc func;
        const ValueP leftEval;
        const ValueP rightEval;
        bool *result;
        const int chunk_start;
        const int chunk_end;

    public:
        PartialCompareBexp(const CompBexpFunc& func, const ValueP& leftEval, const ValueP& rightEval, bool *result, const int chunk_start, const int chunk_end):
            func(func),leftEval(leftEval),rightEval(rightEval),result(result),chunk_start(chunk_start),chunk_end(chunk_end) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const {};
        void writesTo(VarSet& set) const {};
};

class PartialForComm : public Comm {
    private:
        const string loopVarName;
        const int chunk_start;
        const int chunk_end;
        const int step;
        const CommP body;
    
    public:
        PartialForComm(string loopVarName, const int chunk_start, const int chunk_end, const int step, const CommP& body):
            loopVarName(loopVarName),chunk_start(chunk_start),chunk_end(chunk_end),step(step),body(body) {};
        void eval(Store& store, int tid) const;
        void readsFrom(VarSet& set) const {};
        void writesTo(VarSet& set) const {};
};

#endif