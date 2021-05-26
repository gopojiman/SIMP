#ifndef DDG_H
#define DDG_H

#include <list>
#include <string>
using namespace std;

typedef struct ArrayNumRefStruct {
    const string name;
    const int index;
} ANRS;

typedef struct ArrayLoopRefStruct {
    const string name;
    const string loopVar;
} ALRS;

// Represents a set of Vars and Array Refs
class varSet {
    public:
        list<const string> valueList;
        list<const ANRS> anrList;
        list<const ALRS> alrList;
};

#endif