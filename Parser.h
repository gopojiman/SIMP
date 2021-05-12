#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "Comm.h"

class Parser {
    private:
        vector<string> tokens;
        int skipToMatchingParen(int i, int start);
        int skipToMatchingBracket(int i, int start);
        int skipToMatchingElse(int i, int end);
        AexpP parseAexp(int start, int end);
        BexpP parseBexp(int start, int end);
        CommP parseComm(int start, int end);

    public:
        Parser(const string& str);
        CommP parseComm();
};

#endif