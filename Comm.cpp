#include "Comm.h"
#include "Util.h"

void SkipComm::eval(Store& store, int tid) const {
    return;
}

void AssignComm::eval(Store& store, int tid) const {
    store.put(varName, aexp->eval(store, tid));
}

void AssignComm::readsFrom(varSet& set) const {
    aexp->readsFrom(set);
}

// If aexp evals to an array, the first element is used
void AssignNumRefComm::eval(Store& store, int tid) const {
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignNumRefComm::readsFrom(varSet& set) const {
    aexp->readsFrom(set);
}

void AssignLoopRefComm::eval(Store& store, int tid) const {
    int index = store.loopVarMap[loopVar]->at(tid);
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignLoopRefComm::readsFrom(varSet& set) const {
    aexp->readsFrom(set);
}

void SeqComm::eval(Store& store, int tid) const {
    left->eval(store, tid);
    right->eval(store, tid);
}

void SeqComm::readsFrom(varSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}

void IfComm::eval(Store& store, int tid) const {
    if (cond->eval(store, tid)) {
        trueComm->eval(store, tid);
    }
    else {
        falseComm->eval(store, tid);
    }
}

void IfComm::readsFrom(varSet& set) const {
    cond->readsFrom(set);
    trueComm->readsFrom(set);
    falseComm->readsFrom(set);
}

void WhileComm::eval(Store& store, int tid) const {
    while (cond->eval(store, tid)) {
        body->eval(store, tid);
    }
}

void WhileComm::readsFrom(varSet& set) const {
    cond->readsFrom(set);
    body->readsFrom(set);
}

// If start, end, or step eval to arrays, first element is used
void ForComm::eval(Store& store, int tid) const {
    int start = this->start->eval(store, tid)->val();
    int end   = this->end  ->eval(store, tid)->val();
    int step  = this->step ->eval(store, tid)->val();

    store.loopVarMap[loopVarName] = ArrP(new vector<int>(Util::n_threads));
    int& loopVar = store.loopVarMap[loopVarName]->at(tid);
    loopVar = start;
    
    if (step == 0) {
        body->eval(store, tid);
    }
    else if (step > 0) {
        for (; loopVar < end; loopVar += step) {
            body->eval(store, tid);
        }
    }
    else {
        for (; loopVar > end; loopVar += step) {
            body->eval(store, tid);
        }
    }
}

void ForComm::readsFrom(varSet& set) const {
    start->readsFrom(set);
    end  ->readsFrom(set);
    step ->readsFrom(set);
    body ->readsFrom(set);
}