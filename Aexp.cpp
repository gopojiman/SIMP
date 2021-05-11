#include "Aexp.h"

int NumExpr::eval(Store& store) const {
    return val;
}

int VarExpr::eval(Store& store) const {
    return store.getVar(name);
}

int AddExpr::eval(Store& store) const {
    return left->eval(store) + right->eval(store);
}

int SubExpr::eval(Store& store) const {
    return left->eval(store) - right->eval(store);
}

int MultExpr::eval(Store& store) const {
    return left->eval(store) * right->eval(store);
}