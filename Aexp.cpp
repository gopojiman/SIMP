#include <string>
#include "Aexp.h"
#include "Store.h"

// NumExpr
NumExpr::NumExpr(int val) {
    this->val = val;
}

int NumExpr::eval(Store *store) {
    return val;
}

Aexp *NumExpr::copy() {
    return new NumExpr(val);
}

string NumExpr::show() {
    return to_string(val);
}

// VarExpr
VarExpr::VarExpr(string name) {
    this->name = name;
}

int VarExpr::eval(Store *store) {
    return store->getVar(name);
}

Aexp *VarExpr::copy() {
    return new VarExpr(string(name));
}

string VarExpr::show() {
    return name;
}

// AddExpr
AddExpr::AddExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

int AddExpr::eval(Store *store) {
    return left->eval(store) + right->eval(store);
}

Aexp *AddExpr::copy() {
    return new AddExpr(left->copy(), right->copy());
}

string AddExpr::show() {
    return "(" + left->show() + "+" + right->show() + ")";
}

AddExpr::~AddExpr() {
    delete left;
    delete right;
}

// SubExpr
SubExpr::SubExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

int SubExpr::eval(Store *store) {
    return left->eval(store) - right->eval(store);
}

Aexp *SubExpr::copy() {
    return new SubExpr(left->copy(), right->copy());
}

string SubExpr::show() {
    return "(" + left->show() + "-" + right->show() + ")";
}

SubExpr::~SubExpr() {
    delete left;
    delete right;
}

// MultExpr
MultExpr::MultExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

int MultExpr::eval(Store *store) {
    return left->eval(store) * right->eval(store);
}

Aexp *MultExpr::copy() {
    return new MultExpr(left->copy(), right->copy());
}

string MultExpr::show() {
    return "(" + left->show() + "*" + right->show() + ")";
}

MultExpr::~MultExpr() {
    delete left;
    delete right;
}