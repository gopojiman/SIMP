#include "Comm.h"

void SkipComm::eval(Store& store) const {
    return;
}

void AssignComm::eval(Store& store) const {
    store.putVar(varName, aexpr->eval(store));
}

void SeqComm::eval(Store& store) const {
    left->eval(store);
    right->eval(store);
}

void IfComm::eval(Store& store) const {
    if (cond->eval(store)) {
        trueComm->eval(store);
    }
    else {
        falseComm->eval(store);
    }
}

void WhileComm::eval(Store& store) const {
    while (cond->eval(store)) {
        body->eval(store);
    }
}