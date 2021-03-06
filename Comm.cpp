#include "Comm.h"
#include "Util.h"

void Task::eval(Store& store, int tid) {
    comm->eval(store, tid);
    done.store(true);
    for (auto child : children) {
        int result = child->decrement_parents();
        if (result <= 0) {
            Util::workQueue.enqueue(child);
        }
    }
}

int Task::decrement_parents() {
    const int prev_val = atomic_fetch_add(&num_parents, -1);
    return prev_val - 1;
}

void SkipComm::eval(Store& store, int tid) const {
    return;
}

void AssignComm::eval(Store& store, int tid) const {
    store.put(varName, aexp->eval(store, tid));
}

void AssignComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignComm::writesTo(VarSet& set) const {
    set.valueList.push_back(varName);
}

// If aexp evals to an array, the first element is used
void AssignNumRefComm::eval(Store& store, int tid) const {
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignNumRefComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignNumRefComm::writesTo(VarSet& set) const {
    const ANRS anrs = {varName, index};
    set.anrList.push_back(anrs);
}

void AssignLoopRefComm::eval(Store& store, int tid) const {
    const int index = store.loopVarMap[tid][loopVar];
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignLoopRefComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignLoopRefComm::writesTo(VarSet& set) const {
    const ALRS alrs = {varName, loopVar};
    set.alrList.push_back(alrs);
}

void SeqComm::eval(Store& store, int tid) const {
#ifdef NOPARALLEL
    for (auto comm : comms) {
        comm->eval(store, tid);
    }
#else
    list<TaskP> taskList; // list of Tasks we need to wait for
    list<TaskP> headTaskList; // list of the Tasks with no parents

    // Make the DDG
    for (auto comm : comms) {
        TaskP task(new Task(comm));
        int num_parents = 0;
        for (auto task2 : taskList) {
            if (notInterleavable(task2->comm, task->comm)) {
                task2->children.push_back(task);
                num_parents++;
            }
        }
        taskList.push_back(task);
        if (num_parents == 0) {
            headTaskList.push_back(task);
        }
        else {
            task->init_parents(num_parents);
        }
    }

    // Enqueue all the head tasks
    for (auto headTask : headTaskList) {
        Util::workQueue.enqueue(headTask);
    }

    // we only need to wait for the Tasks with no children,
    // prune the rest
    auto taskIt = taskList.begin();
        while (taskIt != taskList.end()) {
            if (!((*taskIt)->children.empty())) {
                taskIt = taskList.erase(taskIt);
            }
            else {
                ++taskIt;
            }
    }

    // wait for all the tasks to finish and execute tasks
    // in the meantime
    while(!taskList.empty()) {
        TaskP task;
        if (Util::workQueue.try_dequeue(task)) {
            task->eval(store, tid);
        }
        taskIt = taskList.begin();
        while (taskIt != taskList.end()) {
            if ((*taskIt)->isDone()) {
                taskIt = taskList.erase(taskIt);
            }
            else {
                ++taskIt;
            }
        }
    }
#endif
}

void SeqComm::readsFrom(VarSet& set) const {
    for (auto comm : comms) {
        comm->readsFrom(set);
    }
}

void SeqComm::writesTo(VarSet& set) const {
    for (auto comm : comms) {
        comm->writesTo(set);
    }
}

void IfComm::eval(Store& store, int tid) const {
    if (cond->eval(store, tid)) {
        trueComm->eval(store, tid);
    }
    else {
        falseComm->eval(store, tid);
    }
}

void IfComm::readsFrom(VarSet& set) const {
    cond->readsFrom(set);
    trueComm->readsFrom(set);
    falseComm->readsFrom(set);
}

void IfComm::writesTo(VarSet& set) const {
    trueComm->writesTo(set);
    falseComm->writesTo(set);
}

void WhileComm::eval(Store& store, int tid) const {
    while (cond->eval(store, tid)) {
        body->eval(store, tid);
    }
}

void WhileComm::readsFrom(VarSet& set) const {
    cond->readsFrom(set);
    body->readsFrom(set);
}

void WhileComm::writesTo(VarSet& set) const {
    body->writesTo(set);
}

// If start, end, or step eval to arrays, first element is used
void ForComm::eval(Store& store, int tid) const {
    const int start = this->start->eval(store, tid)->val();
    const int end   = this->end  ->eval(store, tid)->val();
    const int step  = this->step ->eval(store, tid)->val();

#ifdef NOPARALLEL
    int& loopVar = store.loopVarMap[tid][loopVarName];
    loopVar = start;
    
    if (step == 0) {
        body->eval(store, tid);
    }
    else if (step > 0) {
        for (; loopVar < end; loopVar += step) {
            body->eval(store, tid);
        }
    }
    else {
        for (; loopVar > end; loopVar += step) {
            body->eval(store, tid);
        }
    }
#else
    if (step == 0) {
        store.loopVarMap[tid][loopVarName] = start;
        body->eval(store, tid);
    }
    else {
        const int chunk_size = ((end - start) / Util::n_threads) / step;
        if (notParallelizableLoopBody(body, start, end, step) || chunk_size < MIN_CHUNK_SIZE) {
            // not parellelizing
            int& loopVar = store.loopVarMap[tid][loopVarName];
            loopVar = start;
            
            if (step > 0) {
                for (; loopVar < end; loopVar += step) {
                    body->eval(store, tid);
                }
            }
            else {
                for (; loopVar > end; loopVar += step) {
                    body->eval(store, tid);
                }
            }
        }
        else {
            //parallelizing
            TaskP taskArr[Util::n_threads];
            for (int i = 0; i < Util::n_threads; i++) {
                const int chunk_start = i * chunk_size * step + start;
                const int chunk_end = 
                    (i == (Util::n_threads - 1)) ? end : ((i + 1) * chunk_size * step + start);
                taskArr[i] = TaskP(new Task(
                                CommP(new PartialForComm(
                                    loopVarName, chunk_start, chunk_end, step, body))));
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
    }
#endif
}

void ForComm::readsFrom(VarSet& set) const {
    start->readsFrom(set);
    end  ->readsFrom(set);
    step ->readsFrom(set);
    body ->readsFrom(set);
}

void ForComm::writesTo(VarSet& set) const {
    body->writesTo(set);
}

void PartialBinaryAexp::eval(Store& store, int tid) const {
    for (int i = chunk_start; i < chunk_end; i++) {
        result->put(i, func(leftEval->at(i), rightEval->at(i)));
    }
}

void PartialCompareBexp::eval(Store& store, int tid) const {
    for (int i = chunk_start; i < chunk_end; i++) {
        if (!func(leftEval->at(i), rightEval->at(i))) {
            *result = false;
            return;
        }
    }
    *result = true;
}

void PartialForComm::eval(Store& store, int tid) const {
    int& loopVar = store.loopVarMap[tid][loopVarName];
    loopVar = chunk_start;
    
    if (step > 0) {
        for (; loopVar < chunk_end; loopVar += step) {
            body->eval(store, tid);
        }
    }
    else {
        for (; loopVar > chunk_end; loopVar += step) {
            body->eval(store, tid);
        }
    }
}

bool check(const VarSet& vs1, const VarSet& vs2) {
    for (auto x : vs2.valueList) {
        for (auto y : vs1.valueList) {
            if (x == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x == y.name) return true;
        }
        for (auto y : vs1.alrList) {
            if (x == y.name) return true;
        }
    }
    for (auto x : vs2.anrList) {
        for (auto y : vs1.valueList) {
            if (x.name == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x.name == y.name && x.index == y.index) return true;
        }
        for (auto y : vs1.alrList) {
            if (x.name == y.name) return true;
        }
    }
    for (auto x : vs2.alrList) {
        for (auto y : vs1.valueList) {
            if (x.name == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x.name == y.name) return true;
        }
        for (auto y : vs1.alrList) {
            if (x.name == y.name) return true;
        }
    }
    return false;
}

bool notInterleavable(const CommP& c1, const CommP& c2) {
    VarSet c1Reads, c1Writes, c2Reads, c2Writes;
    c1->readsFrom(c1Reads);
    c1->writesTo(c1Writes);
    c2->readsFrom(c2Reads);
    c2->writesTo(c2Writes);

    return (check(c1Writes, c2Reads) || check(c1Writes, c2Writes) || check(c1Reads, c2Writes));
}

bool checkWithinLoop(const VarSet& vs1, const VarSet& vs2, const int start, const int end, const int step) {
    for (auto x : vs2.valueList) {
        for (auto y : vs1.valueList) {
            if (x == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x == y.name) return true;
        }
        for (auto y : vs1.alrList) {
            if (x == y.name) return true;
        }
    }
    for (auto x : vs2.anrList) {
        for (auto y : vs1.valueList) {
            if (x.name == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x.name == y.name && x.index == y.index) return true;
        }
        for (auto y : vs1.alrList) {
            if (x.name == y.name) {
                if (step > 0 && x.index >= start && x.index < end && (x.index - start) % step == 0) {
                    return true;
                }
                if (step < 0 && x.index <= start && x.index > end && (x.index - start) % step == 0) {
                    return true;
                }
            }
        }
    }
    for (auto x : vs2.alrList) {
        for (auto y : vs1.valueList) {
            if (x.name == y) return true;
        }
        for (auto y : vs1.anrList) {
            if (x.name == y.name) {
                if (step > 0 && y.index >= start && y.index < end && (y.index - start) % step == 0) {
                    return true;
                }
                if (step < 0 && y.index <= start && y.index > end && (y.index - start) % step == 0) {
                    return true;
                }
            }
        }
        for (auto y : vs1.alrList) {
            if (x.name == y.name && x.loopVar != y.loopVar) return true;
        }
    }
    return false;
}

bool notParallelizableLoopBody(const CommP& body, const int start, const int end, const int step) {
    VarSet reads, writes;
    body->readsFrom(reads);
    body->writesTo(writes);

    return (checkWithinLoop(writes, reads, start, end, step) || checkWithinLoop(writes, writes, start, end, step));
}