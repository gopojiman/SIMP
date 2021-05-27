#ifndef COMM_H
#define COMM_H

#include "Bexp.h"
#include "concurrentqueue.h"

class Comm;
class Task;
typedef shared_ptr<const Comm> CommP;
typedef shared_ptr<Task> TaskP;
typedef moodycamel::ConcurrentQueue<TaskP> CQ;

// Wrapper class for Comm that sets an atomic boolean flag
// and decrements its children's num_parents value after it's evaluated. 
// Automatically enqueues children to the workQueue if num_parents is
// decreased to 0.
class Task {
    private:    
        atomic_bool done;
        atomic_int num_parents;
        CQ& workQueue;
    
    public:
        const CommP comm; 
        list<TaskP> children;
        Task(CommP comm, CQ& workQueue):
            comm(comm),done(false),num_parents(0),workQueue(workQueue) {};
        bool isDone() {return done.load();};
        void eval(Store& store, int tid);
        int decrement_parents();
        void init_parents(int n_parents) {num_parents.store(n_parents);};
};

class Comm {
    public:
        virtual void eval(Store& store, int tid, CQ& workQueue) const = 0;
        virtual void readsFrom(VarSet& set) const = 0;
        virtual void writesTo(VarSet& set) const = 0;
        virtual ~Comm() = default;
};

// true -> c2 depends on c1
bool notInterleavable(const CommP& c1, const CommP& c2);

class SkipComm: public Comm {
    public:
        void eval(Store& store, int tid, CQ& workQueue) const;
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
        void eval(Store& store, int tid, CQ& workQueue) const;
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
        void eval(Store& store, int tid, CQ& workQueue) const;
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
        void eval(Store& store, int tid, CQ& workQueue) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

class SeqComm: public Comm {
    private:
        const list<CommP> comms;

    public:
        SeqComm(list<CommP> comms): comms(comms) {};
        void eval(Store& store, int tid, CQ& workQueue) const;
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
        void eval(Store& store, int tid, CQ& workQueue) const;
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
        void eval(Store& store, int tid, CQ& workQueue) const;
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
        void eval(Store& store, int tid, CQ& workQueue) const;
        void readsFrom(VarSet& set) const;
        void writesTo(VarSet& set) const;
};

// All the below classes do not correspond to specific command types in
// the language, they are intermediary constructs used for parallelism

//  TO_IMPLEMENT

#endif