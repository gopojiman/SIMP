#include "Parser.h"

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

// given a "then" at position i, iterates
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

AexpP Parser::parseAexp(int start, int end) {
    if (start == end) {
        string token = tokens[start];
        if (!token.empty() && 
            ((token.at(0) == '-' && token.find_first_not_of("1234567890", 1) == string::npos) ||
            (token.find_first_not_of("1234567890") == string::npos))) {
                return AexpP(new Num(stoi(token)));
            }
        else {
            return AexpP(new Var(token));
        }
    }

    int i = end;
    while (i >= start) {
        if (tokens[i] == "+") {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return AexpP(new BinaryAexp(Aexp::binaryFuncs["+"], left, right));
        }
        if (tokens[i] == "-") {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return AexpP(new BinaryAexp(Aexp::binaryFuncs["-"], left, right));
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
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return AexpP(new BinaryAexp(Aexp::binaryFuncs["*"], left, right));
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

BexpP Parser::parseBexp(int start, int end) {
    if (start == end) {
        string token = tokens[start];
        if (token == "true") {
            return BexpP(new TrueExpr());
        }
        else if (token == "false") {
            return BexpP(new FalseExpr());
        }
    }

    int i = end;
    while (i >= start) {
        if (tokens[i] == "&&") {
            BexpP left = parseBexp(start, i - 1);
            BexpP right = parseBexp(i + 1, end);
            return BexpP(new AndExpr(left, right));
        }
        if (tokens[i] == "||") {
            BexpP left = parseBexp(start, i - 1);
            BexpP right = parseBexp(i + 1, end);
            return BexpP(new OrExpr(left, right));
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
        BexpP oper = parseBexp(start + 1, end);
        return BexpP(new NotExpr(oper));
    }

    i = end;
    while (i >= start) {
        if (tokens[i] == "==") {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return BexpP(new EqExpr(left, right));
        }
        if (tokens[i] == "<") {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return BexpP(new LessExpr(left, right));
        }
        i--;
    }

    cerr << "Error: Unable to parse Bexp" << endl;
    exit(1);
}

CommP Parser::parseComm() {
    return parseComm(0, tokens.size() - 1);
}

CommP Parser::parseComm(int start, int end) {
    if (start == end && tokens[start] == "skip") {
        return CommP(new SkipComm());
    }

    int i = end;
    while (i >= start) {
        if (tokens[i] == ";") {
            CommP left = parseComm(start, i - 1);
            CommP right = parseComm(i + 1, end);
            return CommP(new SeqComm(left, right));
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
                BexpP cond = parseBexp(start + 1, i - 1);
                CommP trueComm = parseComm(i + 1, nextElse - 1);
                CommP falseComm = parseComm(nextElse + 1, end);
                return CommP(new IfComm(cond, trueComm, falseComm));
            }
        }
    }

    if (end - start > 2 && tokens[start] == "while") {
        for (i = start + 2; i < end; i++) {
            if (tokens[i] == "do") {
                BexpP cond = parseBexp(start + 1, i - 1);
                CommP body = parseComm(i + 1, end);
                return CommP(new WhileComm(cond, body));
            }
        }
    }

    if (end - start > 1 && tokens[start + 1] == "=") {
        AexpP aexpr = parseAexp(start + 2, end);
        return CommP(new AssignComm(tokens[start], aexpr));
    }

    cerr << "Error: Unable to parse Comm" << endl;
    exit(1);
}