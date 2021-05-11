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
IfComm::IfComm(Bexp *cond, Comm *trueComm, Comm *falseComm) {
    this->cond = cond;
    this->trueComm = trueComm;
    this->falseComm = falseComm;
}

void IfComm::eval(Store& store) {
    if (cond->eval(store)) {
        trueComm->eval(store);
    }
    else {
        falseComm->eval(store);
    }
}

IfComm::~IfComm() {
    delete cond;
    delete trueComm;
    delete falseComm;
}

// WhileComm
WhileComm::WhileComm(Bexp *cond, Comm *body) {
    this->cond = cond;
    this->body = body;
}

void WhileComm::eval(Store& store) {
    while (cond->eval(store)) {
        body->eval(store);
    }
}

WhileComm::~WhileComm() {
    delete cond;
    delete body;
}