#include "Bexp.h"

bool TrueExpr::eval(Store& store) const {
    return true;
}

bool FalseExpr::eval(Store& store) const {
    return false;
}

bool EqExpr::eval(Store& store) const {
    return left->eval(store) == right->eval(store);
}

bool LessExpr::eval(Store& store) const {
    return left->eval(store) < right->eval(store);
}

bool NotExpr::eval(Store& store) const {
    return !(oper->eval(store));
}

bool AndExpr::eval(Store& store) const {
    return left->eval(store) && right->eval(store);
}

bool OrExpr::eval(Store& store) const {
    return left->eval(store) || right->eval(store);
}