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

void AssignComm::writesTo(varSet& set) const {
    set.valueList.push_back(varName);
}

// If aexp evals to an array, the first element is used
void AssignNumRefComm::eval(Store& store, int tid) const {
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignNumRefComm::readsFrom(varSet& set) const {
    aexp->readsFrom(set);
}

void AssignNumRefComm::writesTo(varSet& set) const {
    ANRS anrs = {varName, index};
    set.anrList.push_back(anrs);
}

void AssignLoopRefComm::eval(Store& store, int tid) const {
    int index = store.loopVarMap[loopVar]->at(tid);
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignLoopRefComm::readsFrom(varSet& set) const {
    aexp->readsFrom(set);
}

void AssignLoopRefComm::writesTo(varSet& set) const {
    ALRS alrs = {varName, loopVar};
    set.alrList.push_back(alrs);
}

void SeqComm::eval(Store& store, int tid) const {
    left->eval(store, tid);
    right->eval(store, tid);
}

void SeqComm::readsFrom(varSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}

void SeqComm::writesTo(varSet& set) const {
    left->writesTo(set);
    right->writesTo(set);
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

void IfComm::writesTo(varSet& set) const {
    trueComm->writesTo(set);
    falseComm->writesTo(set);
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

void WhileComm::writesTo(varSet& set) const {
    body->writesTo(set);
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

void ForComm::writesTo(varSet& set) const {
    body->writesTo(set);
}