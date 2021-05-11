#include <string>
#include "Aexp.h"
#include "Store.h"

// NumExpr
int NumExpr::eval(Store& store) const {
    return val;
}

// VarExpr
int VarExpr::eval(Store& store) const {
    return store.getVar(name);
}

// AddExpr
int AddExpr::eval(Store& store) const {
    return left->eval(store) + right->eval(store);
}

AddExpr::~AddExpr() {
    delete left;
    delete right;
}

// SubExpr
int SubExpr::eval(Store& store) const {
    return left->eval(store) - right->eval(store);
}

SubExpr::~SubExpr() {
    delete left;
    delete right;
}

// MultExpr
int MultExpr::eval(Store& store) const {
    return left->eval(store) * right->eval(store);
}

MultExpr::~MultExpr() {
    delete left;
    delete right;
}