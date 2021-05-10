#include <string>
#include "Aexp.h"
#include "Bexp.h"
#include "Comm.h"
#include "Store.h"

void Comm::obliterate() {
    delete this;
}

// SkipComm
Comm *SkipComm::eval(Store *store) {
    delete this;
    return NULL;
}

Comm *SkipComm::copy() {
    return new SkipComm();
}

string SkipComm::show() {
    return "skip";
}

// AssignComm
AssignComm::AssignComm(string varName, Aexp *aexpr) {
    this->varName = varName;
    this->aexpr = aexpr;
}

Comm *AssignComm::eval(Store *store) {
    store->put(varName, aexpr->eval(store));
    delete this;
    return new SkipComm();
}

Comm *AssignComm::copy() {
    return new AssignComm(string(varName), aexpr->copy());
}

string AssignComm::show() {
    return varName + " := " + aexpr->show();
}

AssignComm::~AssignComm() {
    delete aexpr;
}

// SeqComm
SeqComm::SeqComm(Comm *left, Comm *right) {
    this->left = left;
    this->right = right;
}

Comm *SeqComm::eval(Store *store) {
    Comm *leftEval = left->eval(store);
    if (leftEval == NULL) {
        Comm *rightComm = right;
        delete this;
        return rightComm;
    }
    left = leftEval;
    return this;
}

Comm *SeqComm::copy() {
    return new SeqComm(left->copy(), right->copy());
}

string SeqComm::show() {
    return left->show() + "; " + right->show();
}

void SeqComm::obliterate() {
    left->obliterate();
    right->obliterate();
    delete this;
}

// IfComm
IfComm::IfComm(Bexp *cond, Comm *trueComm, Comm *falseComm) {
    this->cond = cond;
    this->trueComm = trueComm;
    this->falseComm = falseComm;
}

Comm *IfComm::eval(Store *store) {
    if (cond->eval(store)) {
        Comm *tComm = trueComm;
        delete cond;
        falseComm->obliterate();
        delete this;
        return tComm;
    }
    Comm *fComm = falseComm;
    delete cond;
    trueComm->obliterate();
    delete this;
    return fComm;
}

Comm *IfComm::copy() {
    return new IfComm(cond->copy(), trueComm->copy(), falseComm->copy());
}

string IfComm::show() {
    return "if " + cond->show() + " then { " + trueComm->show() + " } else { " + falseComm->show() + " }";
}

void IfComm::obliterate() {
    delete cond;
    trueComm->obliterate();
    falseComm->obliterate();
    delete this;
}

// WhileComm
WhileComm::WhileComm(Bexp *cond, Comm *body) {
    this->cond = cond;
    this->body = body;
}

Comm *WhileComm::eval(Store *store) {
    if (cond->eval(store)) {
        return new SeqComm(body->copy(), this);
    }
    delete this;
    return new SkipComm();
}

Comm *WhileComm::copy() {
    return new WhileComm(cond->copy(), body->copy());
}

string WhileComm::show() {
    return "while " + cond->show() + " do { " + body->show() + " }";
}

WhileComm::~WhileComm() {
    delete cond;
    delete body;
}