#include <string>
#include "Aexp.h"
#include "Store.h"

// NumExpr
NumExpr::NumExpr(int val) {
    this->val = val;
}

int NumExpr::eval(Store& store) {
    return val;
}

// VarExpr
VarExpr::VarExpr(string name) {
    this->name = name;
}

int VarExpr::eval(Store& store) {
    return store.getVar(name);
}

// AddExpr
AddExpr::AddExpr(Aexp *left, Aexp *right) {
    this->left = left;
    this->right = right;
}

int AddExpr::eval(Store& store) {
    return left->eval(store) + right->eval(store);
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

int SubExpr::eval(Store& store) {
    return left->eval(store) - right->eval(store);
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

int MultExpr::eval(Store& store) {
    return left->eval(store) * right->eval(store);
}

MultExpr::~MultExpr() {
    delete left;
    delete right;
}