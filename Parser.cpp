#include <vector>
#include <string>
#include <iostream>
#include "Parser.h"
#include "Aexp.h"
#include "Bexp.h"
#include "Comm.h"
using namespace std;

Parser::Parser(const string& str) {
    char *c_str = new char[str.length() + 1];
    strcpy(c_str, str.c_str());
    char *token = strtok(c_str, " ");

    while (token != NULL) {
        tokens.push_back(string(token));
        token = strtok(NULL, " ");
    }
    delete[] c_str;
}

// given a closing parenthese at position i, iterates
// backwards to find the matching opening parenthese
int Parser::skipToMatchingParen(int i, int start) {
    int j = i - 1;
    while (j >= start) {
        if (tokens[j] == "(") {
            return j;
        }
        if (tokens[j] == ")") {
            j = skipToMatchingParen(j, start);
        }
        j--;
    }
    cerr << "Error: Matching Parenthese Not Found" << endl;
    exit(1);
}

// given a closing bracket at position i, iterates
// backwards to find the matching opening bracket
int Parser::skipToMatchingBracket(int i, int start) {
    int j = i - 1;
    while (j >= start) {
        if (tokens[j] == "{") {
            return j;
        }
        if (tokens[j] == "}") {
            j = skipToMatchingBracket(j, start);
        }
        j--;
    }
    cerr << "Error: Matching Bracket Not Found" << endl;
    exit(1);
}

// given a closing "then" at position i, iterates
// forwards to find the matching "else"
int Parser::skipToMatchingElse(int i, int end) {
    int j = i + 2;
    while (j <= end) {
        if (tokens[j] == "else") {
            return j;
        }
        if (tokens[j] == "then") {
            j = skipToMatchingElse(j, end);
        }
        j++;
    }
    cerr << "Error: Matching Else Not Found" << endl;
    exit(1);
}

Aexp *Parser::parseAexp(int start, int end) {
    if (start == end) {
        string token = tokens[start];
        if (!token.empty() && 
            ((token.at(0) == '-' && token.find_first_not_of("1234567890", 1) == string::npos) ||
            (token.find_first_not_of("1234567890") == string::npos))) {
                return new NumExpr(stoi(token));
            }
        else {
            return new VarExpr(token);
        }
    }

    int i = end;
    while (i >= start) {
        if (tokens[i] == "+") {
            Aexp *left = parseAexp(start, i - 1);
            Aexp *right = parseAexp(i + 1, end);
            return new AddExpr(left, right);
        }
        if (tokens[i] == "-") {
            Aexp *left = parseAexp(start, i - 1);
            Aexp *right = parseAexp(i + 1, end);
            return new SubExpr(left, right);
        }
        if (tokens[i] == ")") {
            int nextParen = skipToMatchingParen(i, start);
            if (i == end && nextParen == start) {
                return parseAexp(start + 1, end - 1);
            }
            i = nextParen;
        }
        i--;
    }

    i = end;
    while (i >= start) {
        if (tokens[i] == "*") {
            Aexp *left = parseAexp(start, i - 1);
            Aexp *right = parseAexp(i + 1, end);
            return new MultExpr(left, right);
        }
        if (tokens[i] == ")") {
            int nextParen = skipToMatchingParen(i, start);
            if (i == end && nextParen == start) {
                return parseAexp(start + 1, end - 1);
            }
            i = nextParen;
        }
        i--;
    }

    cerr << "Error: Unable to parse Aexp" << endl;
    exit(1);
}

Bexp *Parser::parseBexp(int start, int end) {
    if (start == end) {
        string token = tokens[start];
        if (token == "true") {
            return new TrueExpr();
        }
        else if (token == "false") {
            return new FalseExpr();
        }
    }

    int i = end;
    while (i >= start) {
        if (tokens[i] == "&&") {
            Bexp *left = parseBexp(start, i - 1);
            Bexp *right = parseBexp(i + 1, end);
            return new AndExpr(left, right);
        }
        if (tokens[i] == "||") {
            Bexp *left = parseBexp(start, i - 1);
            Bexp *right = parseBexp(i + 1, end);
            return new OrExpr(left, right);
        }
        if (tokens[i] == ")") {
            int nextParen = skipToMatchingParen(i, start);
            if (i == end && nextParen == start) {
                return parseBexp(start + 1, end - 1);
            }
            i = nextParen;
        }
        i--;
    }

    if (start < end && tokens[start] == "!") {
        return new NotExpr(parseBexp(start + 1, end));
    }

    i = end;
    while (i >= start) {
        if (tokens[i] == "==") {
            Aexp *left = parseAexp(start, i - 1);
            Aexp *right = parseAexp(i + 1, end);
            return new EqExpr(left, right);
        }
        if (tokens[i] == "<") {
            Aexp *left = parseAexp(start, i - 1);
            Aexp *right = parseAexp(i + 1, end);
            return new LessExpr(left, right);
        }
        i--;
    }

    cerr << "Error: Unable to parse Bexp" << endl;
    exit(1);
}

Comm *Parser::parseComm() {
    return parseComm(0, tokens.size() - 1);
}

Comm *Parser::parseComm(int start, int end) {
    if (start == end && tokens[start] == "skip") {
        return new SkipComm();
    }

    int i = end;
    while (i >= start) {
        if (tokens[i] == ";") {
            Comm *left = parseComm(start, i - 1);
            Comm *right = parseComm(i + 1, end);
            return new SeqComm(left, right);
        }
        if (tokens[i] == "}") {
            int nextBrack = skipToMatchingBracket(i, start);
            if (i == end && nextBrack == start) {
                return parseComm(start + 1, end - 1);
            }
            i = nextBrack;
        }
        i--;
    }

    if (end - start > 4 && tokens[start] == "if") {
        for (i = start + 2; i < end - 2; i++) {
            if (tokens[i] == "then") {
                int nextElse = skipToMatchingElse(i, end);
                Bexp *cond = parseBexp(start + 1, i - 1);
                Comm *trueComm = parseComm(i + 1, nextElse - 1);
                Comm *falseComm = parseComm(nextElse + 1, end);
                return new IfComm(cond, trueComm, falseComm);
            }
        }
    }

    if (end - start > 2 && tokens[start] == "while") {
        for (i = start + 2; i < end; i++) {
            if (tokens[i] == "do") {
                Bexp *cond = parseBexp(start + 1, i - 1);
                Comm *body = parseComm(i + 1, end);
                return new WhileComm(cond, body);
            }
        }
    }

    if (end - start > 1 && tokens[start + 1] == "=") {
        Aexp *aexpr = parseAexp(start + 2, end);
        return new AssignComm(tokens[start], aexpr);
    }

    cerr << "Error: Unable to parse Comm" << endl;
    exit(1);
}