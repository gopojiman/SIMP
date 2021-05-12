#include "Aexp.h"

map<string, BinAexpFunc> Aexp::binaryFuncs0 = {
    {"+", plus<int>()},
    {"-", minus<int>()}
};

map<string, BinAexpFunc> Aexp::binaryFuncs1 = {
    {"*", multiplies<int>()}
};

int Num::eval(Store& store) const {
    return val;
}

int Var::eval(Store& store) const {
    return store.getVar(name);
}

int BinaryAexp::eval(Store& store) const {
    return func(left->eval(store), right->eval(store));
}