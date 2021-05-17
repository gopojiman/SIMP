#include "Store.h"

void Store::putVar(const string& key, int val) {
    varMap[key] = val;
}

int Store::getVar(const string& key) {
    auto it = varMap.find(key);
    if (it != varMap.end()) {
        return it->second;
    }
    else {
        return 0;
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