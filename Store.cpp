#include "Store.h"

int Value::val() const {
    return arr->at(0);
}

int Value::at(int pos) const {
    if (length < 0) {
        return arr->at(0);
    }
    if (pos >= 0 && pos < length) {
        return arr->at(pos);
    }
    return 0;
}

void Value::put(int pos, int val) {
    if (pos >=0 && pos < length) {
        arr->at(pos) = val;
    }
}

ostream& operator << (ostream& os, const ValueP& valueP) {
    if (valueP->length < 0) {
        return (os << valueP->val());
    }
    else {
        os << '[';
        for (int i = 0; i < valueP->length - 1; i++) {
            os << valueP->at(i) << ", ";
        }
        if (valueP->length > 0) {
            os << valueP->at(valueP->length - 1);
        }
        return (os << ']');
    }
}

void Store::put(const string& key, ValueP valueP) {
    varMap[key] = move(valueP);
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