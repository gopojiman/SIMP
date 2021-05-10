#include <iostream>
#include <string>
#include "Store.h"
using namespace std;

void Store::put(string key, int val) {
    storeMap[key] = val;
}

int Store::get(string key) {
    auto it = storeMap.find(key);
    if (it != storeMap.end()) {
        return it->second;
    }
    else {
        return 0;
    }
}

ostream& operator << (ostream &os, const Store &store) {
    os << '{';
    auto it = store.storeMap.begin();
    auto end = store.storeMap.end();
    if (it != end) {
        os << it->first << " → " << it->second;
        it++;
    }
    while (it != end) {
        os << ", " << it->first << " → " << it->second;
        it++;
    }
    return (os << '}');
}