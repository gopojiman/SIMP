#include "Comm.h"
#include "Util.h"

void Task::eval(Store& store, int tid, CQ& workQueue) {
    comm->eval(store, tid, workQueue);
    done.store(true);
}

void SkipComm::eval(Store& store, int tid, CQ& workQueue) const {
    return;
}

void AssignComm::eval(Store& store, int tid, CQ& workQueue) const {
    store.put(varName, aexp->eval(store, tid));
}

void AssignComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignComm::writesTo(VarSet& set) const {
    set.valueList.push_back(varName);
}

// If aexp evals to an array, the first element is used
void AssignNumRefComm::eval(Store& store, int tid, CQ& workQueue) const {
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignNumRefComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignNumRefComm::writesTo(VarSet& set) const {
    ANRS anrs = {varName, index};
    set.anrList.push_back(anrs);
}

void AssignLoopRefComm::eval(Store& store, int tid, CQ& workQueue) const {
    int index = store.loopVarMap[loopVar]->at(tid);
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignLoopRefComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignLoopRefComm::writesTo(VarSet& set) const {
    ALRS alrs = {varName, loopVar};
    set.alrList.push_back(alrs);
}

void SeqComm::eval(Store& store, int tid, CQ& workQueue) const {
#ifdef NOPARALLEL
    left->eval(store, tid, workQueue);
    right->eval(store, tid, workQueue);
#else
    if (notInterleavable(left, right)) {
        left->eval(store, tid, workQueue);
        right->eval(store, tid, workQueue);
    }
    else {
        TaskP task(new Task(right));
        workQueue.enqueue(task);
        left->eval(store, tid, workQueue);
        while(!(task->isDone()));
    }
#endif
}

void SeqComm::readsFrom(VarSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}

void SeqComm::writesTo(VarSet& set) const {
    left->writesTo(set);
    right->writesTo(set);
}

void IfComm::eval(Store& store, int tid, CQ& workQueue) const {
    if (cond->eval(store, tid)) {
        trueComm->eval(store, tid, workQueue);
    }
    else {
        falseComm->eval(store, tid, workQueue);
    }
}

void IfComm::readsFrom(VarSet& set) const {
    cond->readsFrom(set);
    trueComm->readsFrom(set);
    falseComm->readsFrom(set);
}

void IfComm::writesTo(VarSet& set) const {
    trueComm->writesTo(set);
    falseComm->writesTo(set);
}

void WhileComm::eval(Store& store, int tid, CQ& workQueue) const {
    while (cond->eval(store, tid)) {
        body->eval(store, tid, workQueue);
    }
}

void WhileComm::readsFrom(VarSet& set) const {
    cond->readsFrom(set);
    body->readsFrom(set);
}

void WhileComm::writesTo(VarSet& set) const {
    body->writesTo(set);
}

// If start, end, or step eval to arrays, first element is used
void ForComm::eval(Store& store, int tid, CQ& workQueue) const {
    int start = this->start->eval(store, tid)->val();
    int end   = this->end  ->eval(store, tid)->val();
    int step  = this->step ->eval(store, tid)->val();

    store.loopVarMap[loopVarName] = ArrP(new vector<int>(Util::n_threads));
    int& loopVar = store.loopVarMap[loopVarName]->at(tid);
    loopVar = start;
    
    if (step == 0) {
        body->eval(store, tid, workQueue);
    }
    else if (step > 0) {
        for (; loopVar < end; loopVar += step) {
            body->eval(store, tid, workQueue);
        }
    }
    else {
        for (; loopVar > end; loopVar += step) {
            body->eval(store, tid, workQueue);
        }
    }
}

void ForComm::readsFrom(VarSet& set) const {
    start->readsFrom(set);
    end  ->readsFrom(set);
    step ->readsFrom(set);
    body ->readsFrom(set);
}

void ForComm::writesTo(VarSet& set) const {
    body->writesTo(set);
}

bool check(const VarSet& vs1, const VarSet& vs2) {
    for (auto x : vs2.valueList) {
        for (auto y : vs1.valueList) {
            if (x == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x == y.name) return true;
        }
        for (auto y : vs1.alrList) {
            if (x == y.name) return true;
        }
    }
    for (auto x : vs2.anrList) {
        for (auto y : vs1.valueList) {
            if (x.name == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x.name == y.name && x.index == y.index) return true;
        }
        for (auto y : vs1.alrList) {
            if (x.name == y.name) return true;
        }
    }
    for (auto x : vs2.alrList) {
        for (auto y : vs1.valueList) {
            if (x.name == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x.name == y.name) return true;
        }
        for (auto y : vs1.alrList) {
            if (x.name == y.name) return true;
        }
    }
    return false;
}

bool notInterleavable(const CommP& c1, const CommP& c2) {
    VarSet c1Reads, c1Writes, c2Reads, c2Writes;
    c1->readsFrom(c1Reads);
    c1->writesTo(c1Writes);
    c2->readsFrom(c2Reads);
    c2->writesTo(c2Writes);

    return (check(c1Writes, c2Reads) || check(c1Writes, c2Writes) || check(c1Reads, c2Writes));
}