#include "Comm.h"

// SkipComm
void SkipComm::eval(Store& store) {
    return;
}

// AssignComm
void AssignComm::eval(Store& store) {
    store.putVar(varName, aexpr->eval(store));
}

// SeqComm
SeqComm::SeqComm(Comm *left, Comm *right) {
    this->left = left;
    this->right = right;
}

void SeqComm::eval(Store& store) {
    left->eval(store);
    right->eval(store);
}

SeqComm::~SeqComm() {
    delete left;
    delete right;
}

// IfComm
void IfComm::eval(Store& store) {
    if (cond->eval(store)) {
        trueComm->eval(store);
    }
    else {
        falseComm->eval(store);
    }
}

IfComm::~IfComm() {
    delete trueComm;
    delete falseComm;
}

// WhileComm
void WhileComm::eval(Store& store) {
    while (cond->eval(store)) {
        body->eval(store);
    }
}

WhileComm::~WhileComm() {
    delete body;
}