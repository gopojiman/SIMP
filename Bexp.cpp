#include "Bexp.h"

map<string, CompBexpFunc> Bexp::compFuncs = {
    {"==", equal_to<int>()},
    {"!=", not_equal_to<int>()},
    {">",  greater<int>()},
    {"<",  less<int>()},
    {">=", greater_equal<int>()},
    {"<=", less_equal<int>()}
};

map<string, LogBexpFunc> Bexp::logFuncs = {
    {"&&", logical_and<bool>()},
    {"||", logical_or<bool>()},
    {"^^", [](bool b1, bool b2){return (b1 && !b2) || (!b1 && b2);}} // xor
};

bool LiteralBexp::eval(Store& store) const {
    return val;
}

bool CompareBexp::eval(Store& store) const {
    const ValueP& leftEval = left->eval(store);
    const ValueP& rightEval = right->eval(store);
    if (leftEval->length < 0 && rightEval->length < 0) {
        return func(leftEval->val(), rightEval->val());
    }
    const int maxLen = max(leftEval->length, rightEval->length);
    for (int i = 0; i < maxLen; i++) {
        if (!func(leftEval->at(i), rightEval->at(i))) {
            return false;
        }
    }
    return true;
}

bool NotBexp::eval(Store& store) const {
    return !(oper->eval(store));
}

bool LogicalBexp::eval(Store& store) const {
    return func(left->eval(store), right->eval(store));
}