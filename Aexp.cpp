#include "Aexp.h"

map<string, BinAexpFunc> Aexp::binaryFuncs0 = {
    {"+", plus<int>()},
    {"-", minus<int>()}
};

map<string, BinAexpFunc> Aexp::binaryFuncs1 = {
    {"*", multiplies<int>()},
    {"/", [](int x, int y){return (y == 0) ? 0 : (x / y);}},
    {"%", [](int x, int y){return (y == 0) ? 0 : (x % y);}}
};

ValueP ValueAexp::eval(Store& store, int tid) const {
    if (length < 0) {
        return ValueP(new Value(val));
    }
    return ValueP(new Value(length, val));
}

ValueP Var::eval(Store& store, int tid) const {
    return store.get(name);
}

ValueP LoopVar::eval(Store& store, int tid) const {
    return ValueP(new Value(store.loopVarMap[name]->at(tid)));
}

ValueP BinaryAexp::eval(Store& store, int tid) const {
    ValueP leftEval = left->eval(store, tid);
    ValueP rightEval = right->eval(store, tid);
    if (leftEval->length < 0 && rightEval->length < 0) {
        int result = func(leftEval->val(), rightEval->val());
        return ValueP(new Value(result));
    }
    const int maxLen = max(leftEval->length, rightEval->length);
    ValueP ret(new Value(maxLen, 0));
    for (int i = 0; i < maxLen; i++) {
        ret->put(i, func(leftEval->at(i), rightEval->at(i)));
    }
    return ret;
}

ValueP ArrayNumRef::eval(Store& store, int tid) const {
    return ValueP(new Value(store.get(name)->at(index)));
}