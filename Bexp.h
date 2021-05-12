#ifndef BEXP_H
#define BEXP_H

#include "Aexp.h"

// Alias for Comparison Bexp Function
typedef function<bool(int, int)> CompBexpFunc;
// Alias for Logical Bexp Function
typedef function<bool(bool, bool)> LogBexpFunc;

class Bexp {
    public:
        // mapping from comparison operators to functions
        static map<string, CompBexpFunc> compFuncs;
        // mapping from logical operators to functions
        static map<string, LogBexpFunc> logFuncs;
        virtual bool eval(Store& store) const = 0;
        virtual ~Bexp() = default;
};

// Alias for Bexp smart pointer
typedef unique_ptr<const Bexp> BexpP;

class LiteralBexp: public Bexp {
    private:
        bool val;

    public:
        LiteralBexp(bool val):val(val) {};
        bool eval(Store& store) const;
};

class CompareBexp: public Bexp {
    private:
        CompBexpFunc func;
        AexpP left;
        AexpP right;
    
    public:
        CompareBexp(CompBexpFunc& func, AexpP& left, AexpP& right):
            func(func),left(move(left)),right(move(right)) {};
        bool eval(Store& store) const;
};

class NotBexp: public Bexp {
    private:
        BexpP oper;

    public:
        NotBexp(BexpP& oper):oper(move(oper)) {};
        bool eval(Store& store) const;
};

class LogicalBexp: public Bexp {
    private:
        LogBexpFunc func;
        BexpP left;
        BexpP right;

    public:
        LogicalBexp(LogBexpFunc& func, BexpP& left, BexpP& right):
            func(func),left(move(left)),right(move(right)) {};
        bool eval(Store& store) const;
};

#endif