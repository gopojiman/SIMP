#include "Parser.h"

Parser::Parser(istream& inputStream) {
    string token;
    inputStream >> token;
    while(inputStream) {
        processToken(token);
        inputStream >> token;
    }
}

CommP Parser::parseComm() {
    return parseComm(0, tokens.size() - 1);
}

void Parser::processToken(string& token) {
    if (token.length() > 1) {
        char front = token.front();
        char back = token.back();
        if (back == ';' || back == '}' || back == ')') {
            token.resize(token.size() - 1);
            processToken(token);
            tokens.push_back(string(1, back));
        }
        else if (front == '{' || front == '(' || front == '!') {
            tokens.push_back(string(1, front));
            token.erase(token.begin());
            processToken(token);
        }
        else {
            tokens.push_back(token);
        }
    }
    else {
        tokens.push_back(token);
    }
}

// true -> token is a valid integer, false otherwise
bool Parser::verifyInt(const string& token) {
    return ((token.at(0) == '-' && token.find_first_not_of("1234567890", 1) == string::npos) ||
            (token.find_first_not_of("1234567890") == string::npos));
}

// given a closing parenthese at position i, iterates
// backwards to find the matching opening parenthese
int Parser::skipToMatchingParen(int i, int start) {
    for (int j = i - 1; j >= start; j--) {
        if (tokens[j] == "(") {
            return j;
        }
        if (tokens[j] == ")") {
            j = skipToMatchingParen(j, start);
        }
    }
    cerr << "Error: Matching Parenthese Not Found" << endl;
    exit(1);
}

// given a closing bracket at position i, iterates
// backwards to find the matching opening bracket
int Parser::skipToMatchingBracket(int i, int start) {
    for (int j = i - 1; j >= start; j--) {
        if (tokens[j] == "{") {
            return j;
        }
        if (tokens[j] == "}") {
            j = skipToMatchingBracket(j, start);
        }
    }
    cerr << "Error: Matching Bracket Not Found" << endl;
    exit(1);
}

// given a "then" at position i, iterates
// forwards to find the matching "else"
int Parser::skipToMatchingElse(int i, int end) {
    for (int j = i + 2; j <= end; j++) {
        if (tokens[j] == "else") {
            return j;
        }
        if (tokens[j] == "then") {
            j = skipToMatchingElse(j, end);
        }
    }
    cerr << "Error: Matching Else Not Found" << endl;
    exit(1);
}

AexpP Parser::parseAexp(int start, int end) {
    if (start == end) {
        string token = tokens[start];
        // Num
        if (!token.empty() && verifyInt(token)) {
                return AexpP(new ValueAexp(-1, stoi(token)));
            }
        // New Array
        else if (token.front() == '[' && token.back() == ']') {
            auto commaPos = token.find(',');
            if (commaPos == string::npos) {
                cerr << "Error: invalid array initialization expression" << endl;
                exit(1);
            }
            const string& lengthStr = token.substr(1, commaPos - 1);
            const string& valStr    = token.substr(commaPos + 1, token.length() - commaPos - 2);
            if (!(verifyInt(lengthStr) && verifyInt(valStr))) {
                cerr << "Error: invalid array initialization expression" << endl;
                exit(1);
            }
            int length = stoi(lengthStr);
            int val = stoi(valStr);
            return AexpP(new ValueAexp(length, val));
        }
        // ArrayRef
        else if (token.back() == ']') {
            auto lBracketPos = token.find('[');
            if (lBracketPos == string::npos) {
                cerr << "Error: invalid array reference expression" << endl;
                exit(1);
            }
            const string& indexStr = token.substr(lBracketPos + 1, token.length() - lBracketPos - 2);
            if (!verifyInt(indexStr)) {
                cerr << "Error: invalid array reference expression" << endl;
                exit(1);
            }
            return AexpP(new ArrayNumRef(token.substr(0, lBracketPos), stoi(indexStr)));
        }
        // Var
        else {
            return AexpP(new Var(token));
        }
    }

    // Binary Aexps with low precedence
    for (int i = end; i >= start; i--) {
        string token = tokens[i];
        auto it = Aexp::binaryFuncs0.find(token);
        if (it != Aexp::binaryFuncs0.end()) {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return AexpP(new BinaryAexp(it->second, left, right));
        }
        if (token == ")") {
            int nextParen = skipToMatchingParen(i, start);
            if (i == end && nextParen == start) {
                return parseAexp(start + 1, end - 1);
            }
            i = nextParen;
        }
    }

    // Binary Aexps with high precedence
    for (int i = end; i >= start; i--) {
        string token = tokens[i];
        auto it = Aexp::binaryFuncs1.find(token);
        if (it != Aexp::binaryFuncs1.end()) {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return AexpP(new BinaryAexp(it->second, left, right));
        }
        if (token == ")") {
            int nextParen = skipToMatchingParen(i, start);
            if (i == end && nextParen == start) {
                return parseAexp(start + 1, end - 1);
            }
            i = nextParen;
        }
    }

    cerr << "Error: Unable to parse Aexp" << endl;
    exit(1);
}

BexpP Parser::parseBexp(int start, int end) {
    // Literal Bexp
    if (start == end) {
        string token = tokens[start];
        if (token == "true") {
            return BexpP(new LiteralBexp(true));
        }
        else if (token == "false") {
            return BexpP(new LiteralBexp(false));
        }
    }

    // Logical Bexp
    for (int i = end; i >= start; i--) {
        string token = tokens[i];
        auto it = Bexp::logFuncs.find(token);
        if (it != Bexp::logFuncs.end()) {
            BexpP left = parseBexp(start, i - 1);
            BexpP right = parseBexp(i + 1, end);
            return BexpP(new LogicalBexp(it->second, left, right));
        }
        if (token == ")") {
            int nextParen = skipToMatchingParen(i, start);
            if (i == end && nextParen == start) {
                return parseBexp(start + 1, end - 1);
            }
            i = nextParen;
        }
    }

    // Not Bexp
    if (start < end && tokens[start] == "!") {
        BexpP oper = parseBexp(start + 1, end);
        return BexpP(new NotBexp(oper));
    }

    // Compare Bexp
    for (int i = end; i >= start; i--) {
        string token = tokens[i];
        auto it = Bexp::compFuncs.find(token);
        if (it != Bexp::compFuncs.end()) {
            AexpP left = parseAexp(start, i - 1);
            AexpP right = parseAexp(i + 1, end);
            return BexpP(new CompareBexp(it->second, left, right));
        }
    }

    cerr << "Error: Unable to parse Bexp" << endl;
    exit(1);
}

CommP Parser::parseComm(int start, int end) {
    if (start == end && tokens[start] == "skip") {
        return CommP(new SkipComm());
    }

    for (int i = end; i >= start; i--) {
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
    }

    if (end - start > 4 && tokens[start] == "if") {
        for (int i = start + 2; i < end - 2; i++) {
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
        for (int i = start + 2; i < end; i++) {
            if (tokens[i] == "do") {
                BexpP cond = parseBexp(start + 1, i - 1);
                CommP body = parseComm(i + 1, end);
                return CommP(new WhileComm(cond, body));
            }
        }
    }

    if (end - start > 1 && tokens[start + 1] == "=") {
        AexpP aexp = parseAexp(start + 2, end);
        const string& startToken = tokens[start];
        if (startToken.back() == ']') {
            auto lBracketPos = startToken.find('[');
            if (lBracketPos == string::npos) {
                cerr << "Error: invalid assign Comm" << endl;
                exit(1);
            }
            const string& indexStr = startToken.substr(lBracketPos + 1, startToken.length() - lBracketPos - 2);
            if (!verifyInt(indexStr)) {
                cerr << "Error: invalid assign Comm" << endl;
                exit(1);
            }
            return CommP(new AssignNumRefComm(startToken.substr(0, lBracketPos), stoi(indexStr), aexp));
        }
        return CommP(new AssignComm(startToken, aexp));
    }

    cerr << "Error: Unable to parse Comm" << endl;
    exit(1);
}