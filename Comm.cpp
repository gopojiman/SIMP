#include "Comm.h"

void SkipComm::eval(Store& store, int tid) const {
    return;
}

void AssignComm::eval(Store& store, int tid) const {
    store.put(varName, aexp->eval(store, tid));
}

// If aexp evals to an array, the first element is used
void AssignNumRefComm::eval(Store& store, int tid) const {
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void SeqComm::eval(Store& store, int tid) const {
    left->eval(store, tid);
    right->eval(store, tid);
}

void IfComm::eval(Store& store, int tid) const {
    if (cond->eval(store, tid)) {
        trueComm->eval(store, tid);
    }
    else {
        falseComm->eval(store, tid);
    }
}

void WhileComm::eval(Store& store, int tid) const {
    while (cond->eval(store, tid)) {
        body->eval(store, tid);
    }
}

void ForComm::eval(Store& store, int tid) const {
    int start = this->start->eval(store, tid)->val();
    int end   = this->end  ->eval(store, tid)->val();
    int step  = this->step ->eval(store, tid)->val();

    //...
}