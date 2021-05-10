#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "Aexp.h"
#include "Bexp.h"
#include "Comm.h"
using namespace std;

class Parser {
    private:
        vector<string> tokens;
        int skipToMatchingParen(int i, int start);
        int skipToMatchingBracket(int i, int start);
        int skipToMatchingElse(int i, int end);
        Aexp *parseAexp(int start, int end);
        Bexp *parseBexp(int start, int end);
        Comm *parseComm(int start, int end);

    public:
        Parser(string str);
        Comm *parseComm(); // allocates a new Comm, must call delete after
};

#endif