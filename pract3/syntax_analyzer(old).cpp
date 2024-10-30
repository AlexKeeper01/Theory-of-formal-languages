#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

const int NUM_OF_KWORDS = 2;
string keywords[NUM_OF_KWORDS] = { "for", "do" };

enum class State { H, ID, NM, ASGN, DLM, ERR };
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

        if (i < len && (input[i] == '+' || input[i] == '-')) {
            i++;
        }

        bool hasDigits = false;
        while (i < len && isdigit(input[i])) {
            i++;
            hasDigits = true;
        }

        if (i < len && input[i] == '.') {
            i++;
            if (!hasDigits) {
                return false;
            }
            while (i < len && isdigit(input[i])) {
                i++;
            }
        }

        if (i < len && (input[i] == 'e' || input[i] == 'E')) {
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

        if (i < len && (input[i] == 'f' || input[i] == 'F' || input[i] == 'l' || input[i] == 'L')) {
            i++;
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
                else if (isdigit(c) || c == '+' || c == '-') {
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
                if (c == '+' || c == '-' || isdigit(c) || c == 'e' || c == 'E' || c == '.' || c == 'f' || c == 'F' || c == 'l' || c == 'L') {
                    currentToken.value += c;
                }
                else {
                    if (isNum(currentToken.value)) {
                        currentToken.type = TokenType::NUM;
                        lexemeTable.addToken(currentToken);
                        currentToken.value.clear();
                        currentState = State::H;
                        file.putback(c);
                    }
                    else {
                        cout << "Wrong num: " << currentToken.value << '\n';
                        currentState = State::ERR;
                    }
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
                if (c == '(' || c == ')' || c == ';') {
                    currentToken.type = TokenType::DELIM;
                    currentToken.value = c;
                    lexemeTable.addToken(currentToken);
                    currentToken.value.clear();
                    currentState = State::H;
                }
                else if (c == '<' || c == '>' || c == '=') {
                    currentToken.type = TokenType::OPER;
                    currentToken.value = c;
                    lexemeTable.addToken(currentToken);
                    currentToken.value.clear();
                    currentState = State::H;
                }
                else {
                    cout << "Unknown character: " << c << '\n';
                    currentState = State::ERR;
                }
                break;

            case State::ERR:
                currentState = State::H;
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
