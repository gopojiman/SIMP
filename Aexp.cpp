#include "Aexp.h"
#include "Comm.h"

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
    const int l = length->eval(store, tid)->val();
    const int v =    val->eval(store, tid)->val();
    if (l < 0) {
        return ValueP(new Value(v));
    }
    return ValueP(new Value(l, v));
}

void ValueAexp::readsFrom(VarSet& set) const {
    length->readsFrom(set);
    val   ->readsFrom(set);
}

ValueP Var::eval(Store& store, int tid) const {
    return store.get(name);
}

void Var::readsFrom(VarSet& set) const {
    set.valueList.push_back(name);
}

ValueP LoopVar::eval(Store& store, int tid) const {
    return ValueP(new Value(store.loopVarMap[tid][name]));
}

ValueP BinaryAexp::eval(Store& store, int tid) const {
    const ValueP leftEval = left->eval(store, tid);
    const ValueP rightEval = right->eval(store, tid);
    if (leftEval->length < 0 && rightEval->length < 0) {
        int result = func(leftEval->val(), rightEval->val());
        return ValueP(new Value(result));
    }
    const int maxLen = max(leftEval->length, rightEval->length);
    ValueP ret(new Value(maxLen, 0));
#ifdef NOPARALLEL
    for (int i = 0; i < maxLen; i++) {
        ret->put(i, func(leftEval->at(i), rightEval->at(i)));
    }
#else
    const int chunk_size = maxLen / Util::n_threads;
    if (chunk_size < MIN_CHUNK_SIZE) {
        for (int i = 0; i < maxLen; i++) {
            ret->put(i, func(leftEval->at(i), rightEval->at(i)));
        }
    }
    else {
        TaskP taskArr[Util::n_threads];
        for (int i = 0; i < Util::n_threads; i++) {
            const int chunk_start = i * chunk_size;
            const int chunk_end = 
                (i == (Util::n_threads - 1)) ? maxLen : ((i + 1) * chunk_size);
            taskArr[i] = TaskP(new Task(
                            CommP(new PartialBinaryAexp(
                                func, leftEval, rightEval, ret, 
                                chunk_start, chunk_end))));
        }
        Util::workQueue.enqueue_bulk(taskArr, Util::n_threads);

        // Wait for all the tasks to finish
        TaskP task;
        for (int i = 0; i < Util::n_threads; i++) {
            while (!taskArr[i]->isDone()) {
                if (Util::workQueue.try_dequeue(task)) {
                    task->eval(store, tid);
                }
            }
        }
    }
#endif
    return ret;
}

void BinaryAexp::readsFrom(VarSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}

ValueP ArrayNumRef::eval(Store& store, int tid) const {
    return ValueP(new Value(store.get(name)->at(index)));
}

void ArrayNumRef::readsFrom(VarSet& set) const {
    const ANRS anrs = {name, index};
    set.anrList.push_back(anrs);
}

ValueP ArrayLoopRef::eval(Store& store, int tid) const {
    const int index = store.loopVarMap[tid][loopVar];
    return ValueP(new Value(store.get(arrayName)->at(index)));
}

void ArrayLoopRef::readsFrom(VarSet& set) const {
    const ALRS alrs = {arrayName, loopVar};
    set.alrList.push_back(alrs);
}