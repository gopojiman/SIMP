#include "Comm.h"
#include "Util.h"

void Task::eval(Store& store, int tid) {
    comm->eval(store, tid, workQueue);
    done.store(true);
    for (auto child : children) {
        int result = child->decrement_parents();
        if (result <= 0) {
            workQueue.enqueue(child);
        }
    }
}

int Task::decrement_parents() {
    const int prev_val = atomic_fetch_add(&num_parents, -1);
    return prev_val - 1;
}

void SkipComm::eval(Store& store, int tid, CQ& workQueue) const {
    return;
}

void AssignComm::eval(Store& store, int tid, CQ& workQueue) const {
    store.put(varName, aexp->eval(store, tid));
}

void AssignComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignComm::writesTo(VarSet& set) const {
    set.valueList.push_back(varName);
}

// If aexp evals to an array, the first element is used
void AssignNumRefComm::eval(Store& store, int tid, CQ& workQueue) const {
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignNumRefComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignNumRefComm::writesTo(VarSet& set) const {
    ANRS anrs = {varName, index};
    set.anrList.push_back(anrs);
}

void AssignLoopRefComm::eval(Store& store, int tid, CQ& workQueue) const {
    int index = store.loopVarMap[tid][loopVar];
    store.get(varName)->put(index, aexp->eval(store, tid)->val());
}

void AssignLoopRefComm::readsFrom(VarSet& set) const {
    aexp->readsFrom(set);
}

void AssignLoopRefComm::writesTo(VarSet& set) const {
    ALRS alrs = {varName, loopVar};
    set.alrList.push_back(alrs);
}

void SeqComm::eval(Store& store, int tid, CQ& workQueue) const {
#ifdef NOPARALLEL
    for (auto comm : comms) {
        comm->eval(store, tid, workQueue);
    }
#else
    list<TaskP> taskList; // list of Tasks we need to wait for

    auto it = comms.begin();
    TaskP headTask(new Task(*it, workQueue));
    taskList.push_back(headTask);

    // create DDG
    TaskP prev = headTask;
    for (++it; it != comms.end(); ++it) {
        TaskP curTask(new Task(*it, 1, workQueue));
        taskList.push_back(curTask);
        prev->children.push_back(curTask);
        prev = curTask;
    }
    workQueue.enqueue(headTask);

    // we only need to wait for the Tasks with no children
    auto taskIt = taskList.begin();
        while (taskIt != taskList.end()) {
            if (!((*taskIt)->children.empty())) {
                taskIt = taskList.erase(taskIt);
            }
            else {
                ++taskIt;
            }
    }

    // wait for all the tasks to finish
    while(!taskList.empty()) {
        TaskP task;
        if (workQueue.try_dequeue(task)) {
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

void IfComm::eval(Store& store, int tid, CQ& workQueue) const {
    if (cond->eval(store, tid)) {
        trueComm->eval(store, tid, workQueue);
    }
    else {
        falseComm->eval(store, tid, workQueue);
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

void WhileComm::eval(Store& store, int tid, CQ& workQueue) const {
    while (cond->eval(store, tid)) {
        body->eval(store, tid, workQueue);
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
void ForComm::eval(Store& store, int tid, CQ& workQueue) const {
    int start = this->start->eval(store, tid)->val();
    int end   = this->end  ->eval(store, tid)->val();
    int step  = this->step ->eval(store, tid)->val();

    int& loopVar = store.loopVarMap[tid][loopVarName];
    loopVar = start;
    
    if (step == 0) {
        body->eval(store, tid, workQueue);
    }
    else if (step > 0) {
        for (; loopVar < end; loopVar += step) {
            body->eval(store, tid, workQueue);
        }
    }
    else {
        for (; loopVar > end; loopVar += step) {
            body->eval(store, tid, workQueue);
        }
    }
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