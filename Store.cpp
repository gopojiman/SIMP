#include "Store.h"

ostream& operator << (ostream& os, const ValueP& valueP) {
    if (valueP->length < 0) {
        return (os << valueP->arr->at(0));
    }
    else {
        for (int i = 0; i < valueP->length; i++) {
            os << valueP->arr->at(i) << " ";
        }
        return os;
    }
}

void Store::putNum(const string& key, int val) {
    ArrP arr(new vector<int>(1, val));
    varMap[key] = ValueP(new Value(-1, arr));
}

ValueP& Store::get(const string& key) {
    auto it = varMap.find(key);
    if (it != varMap.end()) {
        return it->second;
    }
    else {
        return defaultValue;
    }
}

ostream& operator << (ostream& os, const Store& store) {
    os << '{';
    auto it = store.varMap.begin();
    auto end = store.varMap.end();
    if (it != end) {
        os << it->first << " → " << it->second;
        it++;
    }
    while (it != end) {
        os << ",\n " << it->first << " → " << it->second;
        it++;
    }
    return (os << '}');
}