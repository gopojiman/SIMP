#ifndef AEXP_H
#define AEXP_H

#include <memory>
#include "Store.h"

class Aexp {
    public:
        virtual int eval(Store& store) const = 0;
        virtual ~Aexp() = default;
};

class NumExpr: public Aexp {
    private:
        const int val;

    public:
        NumExpr(int val):val(val) {};
        int eval(Store& store) const;
};

class VarExpr: public Aexp {
    private:
        const string name;

    public:
        VarExpr(string name):name(name) {};
        int eval(Store& store) const;
};

class AddExpr: public Aexp {
    private:
        unique_ptr<const Aexp> left;
        unique_ptr<const Aexp> right;
    
    public:
        AddExpr(unique_ptr<const Aexp>& left, unique_ptr<const Aexp>& right):
            left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

class SubExpr: public Aexp {
    private:
        unique_ptr<const Aexp> left;
        unique_ptr<const Aexp> right;
    
    public:
        SubExpr(unique_ptr<const Aexp>& left, unique_ptr<const Aexp>& right):
            left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

class MultExpr: public Aexp {
    private:
        unique_ptr<const Aexp> left;
        unique_ptr<const Aexp> right;
    
    public:
        MultExpr(unique_ptr<const Aexp>& left, unique_ptr<const Aexp>& right):
            left(move(left)),right(move(right)) {};
        int eval(Store& store) const;
};

#endif