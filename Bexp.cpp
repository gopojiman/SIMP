#include <string>
#include "Aexp.h"
#include "Bexp.h"
#include "Store.h"

// TrueExpr
bool TrueExpr::eval(Store *store) {
    return true;
}

// FalseExpr
bool FalseExpr::eval(Store *store) {
    return false;
}

// EqExpr
EqExpr::EqExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

bool EqExpr::eval(Store *store) {
    return left->eval(store) == right->eval(store);
}

EqExpr::~EqExpr() {
    delete left;
    delete right;
}

// LessExpr
LessExpr::LessExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

bool LessExpr::eval(Store *store) {
    return left->eval(store) < right->eval(store);
}

LessExpr::~LessExpr() {
    delete left;
    delete right;
}

// NotExpr
NotExpr::NotExpr(Bexp *oper) {
    this->oper = oper;
}

bool NotExpr::eval(Store *store) {
    return !(oper->eval(store));
}

NotExpr::~NotExpr() {
    delete oper;
}

// AndExpr
AndExpr::AndExpr(Bexp *left, Bexp *right) {
    this->left = left;
    this->right = right;
}

bool AndExpr::eval(Store *store) {
    return left->eval(store) && right->eval(store);
}

AndExpr::~AndExpr() {
    delete left;
    delete right;
}

// OrExpr
OrExpr::OrExpr(Bexp *left, Bexp *right) {
    this->left = left;
    this->right = right;
}

bool OrExpr::eval(Store *store) {
    return left->eval(store) || right->eval(store);
}

OrExpr::~OrExpr() {
    delete left;
    delete right;
}