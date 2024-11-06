#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const int NUM_OF_KWORDS = 15;
string keywords[NUM_OF_KWORDS] = { "dim", "integer", "real", "boolean", "if", "else", "for", "to", "step", "next", "while", "readln", "writeln", "true", "false"};

enum class State { H, ID, NM, ASGN, DLM, ERR, COMM };
enum class TokenType { KWORD, IDENT, NUM, OPER, DELIM };

struct Token {
    TokenType type;
    string value;
};

struct LexemeTable {
    vector<Token> tokens;

    void addToken(const Token& token) {
        tokens.push_back(token);
    }

    void printTokens() const {
        for (const auto& token : tokens) {
            cout << "Token type: ";
            switch ((int)token.type) {
            case 0:
                cout << "KWORD";
                break;
            case 1:
                cout << "IDENT";
                break;
            case 2:
                cout << "NUM";
                break;
            case 3:
                cout << "OPER";
                break;
            case 4:
                cout << "DELIM";
                break;
            }
            cout << ", value: " << token.value << '\n';
        }
    }
};

class Lexer {
public:
    Lexer(const string& filename) : filename(filename) {}

    bool isKeyword(const string& id) {
        for (const string& keyword : keywords) {
            if (id == keyword) {
                return true;
            }
        }
        return false;
    }

    bool isNum(const string& input) {
        size_t i = 0;
        size_t len = input.length();

        if (input[len - 1] == 'B' || input[len - 1] == 'b') {
            for (i = 0; i < len - 1; i++) {
                if (input[i] != '0' && input[i] != '1') {
                    return false;
                }
            }
            return true;
        }
        else if (input[len - 1] == 'O' || input[len - 1] == 'o') {
            for (i = 0; i < len - 1; i++) {
                if (input[i] < '0' || input[i] > '7') {
                    return false;
                }
            }
            return true;
        }
        else if (input[len - 1] == 'D' || input[len - 1] == 'd') {
            for (i = 0; i < len - 1; i++) {
                if (!isdigit(input[i])) {
                    return false;
                }
            }
            return true;
        }
        else if (input[len - 1] == 'H' || input[len - 1] == 'h') {
            for (i = 0; i < len - 1; i++) {
                if (!isdigit(input[i]) && (input[i] < 'A' || input[i] > 'F') && (input[i] < 'a' || input[i] > 'f')) {
                    return false;
                }
            }
            return true;
        }

        i = 0;

        while (i < len && isdigit(input[i])) {
            i++;
        }

        if (len == i) {
            return true;
        }

        if (i < len && input[i] == '.') {
            bool hasDigits = false;
            i++;
            while (i < len && isdigit(input[i])) {
                i++;
                hasDigits = true;
            }
            if (!hasDigits) {
                return false;
            }
        }

        if (i < len && (input[i] == 'E' || input[i] == 'e')) {
            i++;
            if (i < len && (input[i] == '+' || input[i] == '-')) {
                i++;
            }
            bool hasExpDigits = false;
            while (i < len && isdigit(input[i])) {
                i++;
                hasExpDigits = true;
            }

            if (!hasExpDigits) {
                return false;
            }
        }

        return i == len;
    }

    void analyze() {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Cannot open file " << filename << ".\n";
            return;
        }

        char c;
        State currentState = State::H;
        Token currentToken;

        while (file.get(c)) {
            switch (currentState) {
            case State::H:
                if (isspace(c)) continue;
                else if (isalpha(c)) {
                    currentState = State::ID;
                    currentToken.value += c;
                }
                else if (isdigit(c) || c == '.') {
                    currentState = State::NM;
                    currentToken.value += c;
                }
                else if (c == ':') {
                    currentState = State::ASGN;
                }
                else {
                    currentState = State::DLM;
                    file.putback(c);
                }
                break;

            case State::ID:
                if (isalnum(c)) {
                    currentToken.value += c;
                }
                else {
                    currentToken.type = isKeyword(currentToken.value) ? TokenType::KWORD : TokenType::IDENT;
                    lexemeTable.addToken(currentToken);
                    currentToken.value.clear();
                    currentState = State::H;
                    file.putback(c);
                }
                break;

            case State::NM:
                if (isalnum(c) || c == '.' || c == '+' || c == '-' ) {
                    currentToken.value += c;
                }
                else if(isNum(currentToken.value)) {
                    currentToken.type = TokenType::NUM;
                    lexemeTable.addToken(currentToken);
                    currentToken.value.clear();
                    currentState = State::H;
                    file.putback(c);
                }
                else {
                    cout << "Wrong num: " << currentToken.value << '\n';
                    currentToken.value.clear();
                    currentState = State::ERR;
                }
                break;

            case State::ASGN:
                if (c == '=') {
                    currentToken.type = TokenType::OPER;
                    currentToken.value = ":=";
                    lexemeTable.addToken(currentToken);
                    currentToken.value.clear();
                    currentState = State::H;
                }
                else {
                    cout << "Unknown character: " << c << '\n';
                    currentState = State::ERR;
                }
                break;

            case State::DLM:
                if (c == '(' || c == ')' || c == ';' || c == '{' || c == '}' || c == ',') {
                    currentToken.type = TokenType::DELIM;
                    currentToken.value = c;
                    lexemeTable.addToken(currentToken);
                    currentToken.value.clear();
                    currentState = State::H;
                }
                else if (c == '<' || c == '>' || c == '=' || c == '!' || c == '*' || c == '/' || c == '&' || c == '+' || c == '-' || c == '|') {
                    currentToken.value += c;
                }
                else {
                    if (currentToken.value == "/*") {
                        currentState = State::COMM;
                    }
                    else {
                        currentToken.type = TokenType::OPER;
                        lexemeTable.addToken(currentToken);
                        currentToken.value.clear();
                        currentState = State::H;
                    }
                }
                break;

            case State::COMM:
                currentToken.value += c;
                if (currentToken.value[currentToken.value.length() - 3] == '*' && currentToken.value[currentToken.value.length() - 2] == '/') {
                    currentToken.value.clear();
                    currentState = State::H;
                }
                break;

            case State::ERR:
                currentState = State::H;
                file.putback(c);
                break;

            default:
                break;
            }
        }
    }

    void printTokens() const {
        lexemeTable.printTokens();
    }

private:
    string filename;
    LexemeTable lexemeTable;
};

int main() {
    Lexer lexer("input.txt");
    lexer.analyze();
    lexer.printTokens();
    return 0;
}
