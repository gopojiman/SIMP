#include <string>
#include "Aexp.h"
#include "Bexp.h"
#include "Store.h"

// TrueExpr
bool TrueExpr::eval(Store *store) {
    return true;
}

Bexp *TrueExpr::copy() {
    return new TrueExpr();
}

string TrueExpr::show() {
    return "true";
}

// FalseExpr
bool FalseExpr::eval(Store *store) {
    return false;
}

Bexp *FalseExpr::copy() {
    return new FalseExpr();
}

string FalseExpr::show() {
    return "false";
}

// EqExpr
EqExpr::EqExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

bool EqExpr::eval(Store *store) {
    return left->eval(store) == right->eval(store);
}

Bexp *EqExpr::copy() {
    return new EqExpr(left->copy(), right->copy());
}

string EqExpr::show() {
    return "(" + left->show() + "=" + right->show() + ")";
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

Bexp *LessExpr::copy() {
    return new LessExpr(left->copy(), right->copy());
}

string LessExpr::show() {
    return "(" + left->show() + "<" + right->show() + ")";
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

Bexp *NotExpr::copy() {
    return new NotExpr(oper->copy());
}

string NotExpr::show() {
    return "¬" + oper->show();
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

Bexp *AndExpr::copy() {
    return new AndExpr(left->copy(), right->copy());
}

string AndExpr::show() {
    return "(" + left->show() + "∧" + right->show() + ")";
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

Bexp *OrExpr::copy() {
    return new OrExpr(left->copy(), right->copy());
}

string OrExpr::show() {
    return "(" + left->show() + "∨" + right->show() + ")";
}

OrExpr::~OrExpr() {
    delete left;
    delete right;
}