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
    return func(left->eval(store), right->eval(store));
}

bool NotBexp::eval(Store& store) const {
    return !(oper->eval(store));
}

bool LogicalBexp::eval(Store& store) const {
    return func(left->eval(store), right->eval(store));
}