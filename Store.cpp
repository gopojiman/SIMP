#include "Store.h"

int Value::val() const {
    return arr->at(0);
}

int Value::at(int pos) const {
    if (pos >= 0 && pos < length) {
        return arr->at(pos);
    }
    return 0;
}

ostream& operator << (ostream& os, const ValueP& valueP) {
    if (valueP->length < 0) {
        return (os << valueP->val());
    }
    else {
        for (int i = 0; i < valueP->length; i++) {
            os << valueP->at(i) << " ";
        }
        return os;
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