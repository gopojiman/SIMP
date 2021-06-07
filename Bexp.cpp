#include "Bexp.h"
#include "Comm.h"

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

bool LiteralBexp::eval(Store& store, int tid) const {
    return val;
}

bool CompareBexp::eval(Store& store, int tid) const {
    const ValueP& leftEval = left->eval(store, tid);
    const ValueP& rightEval = right->eval(store, tid);
    if (leftEval->length < 0 && rightEval->length < 0) {
        return func(leftEval->val(), rightEval->val());
    }
    const int maxLen = max(leftEval->length, rightEval->length);
#ifdef NOPARALLEL
    for (int i = 0; i < maxLen; i++) {
        if (!func(leftEval->at(i), rightEval->at(i))) {
            return false;
        }
    }
    return true;
#else
    const int chunk_size = maxLen / Util::n_threads;
    if (chunk_size < MIN_CHUNK_SIZE) {
        for (int i = 0; i < maxLen; i++) {
            if (!func(leftEval->at(i), rightEval->at(i))) {
                return false;
            }
        }
        return true;
    }
    else {
        TaskP taskArr[Util::n_threads];
        bool results[Util::n_threads];
        for (int i = 0; i < Util::n_threads; i++) {
            const int chunk_start = i * chunk_size;
            const int chunk_end = 
                (i == (Util::n_threads - 1)) ? maxLen : ((i + 1) * chunk_size);
            taskArr[i] = TaskP(new Task(
                            CommP(new PartialCompareBexp(
                                func, leftEval, rightEval, &results[i], 
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

        for (int i = 0; i < Util::n_threads; i++) {
            if (!results[i]) {
                return false;
            }
        }
        return true;
    }
#endif
}

void CompareBexp::readsFrom(VarSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}

bool NotBexp::eval(Store& store, int tid) const {
    return !(oper->eval(store, tid));
}

void NotBexp::readsFrom(VarSet& set) const {
    oper->readsFrom(set);
}

bool LogicalBexp::eval(Store& store, int tid) const {
    return func(left->eval(store, tid), right->eval(store, tid));
}

void LogicalBexp::readsFrom(VarSet& set) const {
    left->readsFrom(set);
    right->readsFrom(set);
}