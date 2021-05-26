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

ValueP Num::eval(Store& store, int tid) const {
    return ValueP(new Value(val));
}

// If length or val evaluate to arrays, the first element is used
ValueP ValueAexp::eval(Store& store, int tid) const {
    int l = length->eval(store, tid)->val();
    int v =    val->eval(store, tid)->val();
    if (l < 0) {
        return ValueP(new Value(v));
    }
    return ValueP(new Value(l, v));
}

ValueP Var::eval(Store& store, int tid) const {
    return store.get(name);
}

void Var::readsFrom(varSet& set) const {
    set.valueList.push_back(name);
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

void BinaryAexp::readsFrom(varSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}

ValueP ArrayNumRef::eval(Store& store, int tid) const {
    return ValueP(new Value(store.get(name)->at(index)));
}

void ArrayNumRef::readsFrom(varSet& set) const {
    ANRS anrs = {name, index};
    set.anrList.push_back(anrs);
}

ValueP ArrayLoopRef::eval(Store& store, int tid) const {
    int index = store.loopVarMap[loopVar]->at(tid);
    return ValueP(new Value(store.get(arrayName)->at(index)));
}

void ArrayLoopRef::readsFrom(varSet& set) const {
    ALRS alrs = {arrayName, loopVar};
    set.alrList.push_back(alrs);
}