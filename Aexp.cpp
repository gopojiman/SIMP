#include "Aexp.h"

map<string, BinAexpFunc> Aexp::binaryFuncs0 = {
    {"+", plus<int>()},
    {"-", minus<int>()}
};

map<string, BinAexpFunc> Aexp::binaryFuncs1 = {
    {"*", multiplies<int>()}
};

ValueP ValueAexp::eval(Store& store) const {
    if (length < 0) {
        return ValueP(new Value(val));
    }
    return ValueP(new Value(length, val));
}

ValueP Var::eval(Store& store) const {
    return store.get(name);
}

ValueP BinaryAexp::eval(Store& store) const {
    ValueP leftEval = left->eval(store);
    ValueP rightEval = right->eval(store);
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