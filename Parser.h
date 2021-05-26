#ifndef PARSER_H
#define PARSER_H

#include <unordered_set>
#include "Comm.h"

class Parser {
    private:
        vector<string> tokens;
        unordered_set<string> loopVars;
        void processToken(string& token);
        bool verifyInt(const string& token);
        int skipToMatchingParen(int i, int start);
        int skipToMatchingBracket(int i, int start);
        int skipToMatchingElse(int i, int end);
        int skipToMatchingSquareBracket(const string& token, int i);
        AexpP parseAexp(int start, int end);
        BexpP parseBexp(int start, int end);
        CommP parseComm(int start, int end);

    public:
        Parser(istream& inputStream);
        CommP parseComm();
};

#endif