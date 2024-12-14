#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <unordered_map>
#include <stdexcept>
#include <vector>

using namespace std;

enum lex_type {
    LEX_NULL, LEX_DIM, LEX_INTEGER, LEX_REAL, LEX_BOOLEAN,
    LEX_IF, LEX_ELSE, LEX_FOR, LEX_TO, LEX_STEP, LEX_NEXT,
    LEX_WHILE, LEX_READLN, LEX_WRITELN, LEX_TRUE, LEX_FALSE,
    LEX_ID, LEX_NUM, LEX_ASSIGN, LEX_EQ, LEX_NEQ, LEX_LT,
    LEX_LEQ, LEX_GT, LEX_GEQ, LEX_PLUS, LEX_MINUS, LEX_OR,
    LEX_TIMES, LEX_DIV, LEX_AND, LEX_NOT, LEX_BEGIN, LEX_END,
    LEX_FIN, LEX_DELIM
};

struct Lex {
    lex_type type;
    string value;

    Lex(lex_type t = LEX_NULL, const string& v = "") : type(t), value(v) {}
};

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

    if (input[len - 1] == 'O' || input[len - 1] == 'o') {
        for (i = 0; i < len - 1; i++) {
            if (input[i] < '0' || input[i] > '7') {
                return false;
            }
        }
        return true;
    }

    if (input[len - 1] == 'D' || input[len - 1] == 'd') {
        for (i = 0; i < len - 1; i++) {
            if (!isdigit(input[i])) {
                return false;
            }
        }
        return true;
    }

    if (input[len - 1] == 'H' || input[len - 1] == 'h') {
        for (i = 0; i < len - 1; i++) {
            if (!isdigit(input[i]) && (input[i] < 'A' || input[i] > 'F') &&
                (input[i] < 'a' || input[i] > 'f')) {
                return false;
            }
        }
        return true;
    }

    i = 0;
    while (i < len && isdigit(input[i])) {
        i++;
    }

    if (len == i) return true;

    if (i < len && input[i] == '.') {
        bool hasDigits = false;
        i++;
        while (i < len && isdigit(input[i])) {
            i++;
            hasDigits = true;
        }
        if (!hasDigits) return false;
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
        if (!hasExpDigits) return false;
    }

    return i == len;
}

class Lexer {
private:
    ifstream file;
    char current_char;
    unordered_map<string, lex_type> keywords = {
    {"dim", LEX_DIM}, {"integer", LEX_INTEGER}, {"real", LEX_REAL}, {"boolean", LEX_BOOLEAN},
    {"if", LEX_IF}, {"else", LEX_ELSE}, {"for", LEX_FOR}, {"to", LEX_TO}, {"step", LEX_STEP},
    {"next", LEX_NEXT}, {"while", LEX_WHILE}, {"readln", LEX_READLN}, {"writeln", LEX_WRITELN},
    {"true", LEX_TRUE}, {"false", LEX_FALSE}, {"begin", LEX_BEGIN}, {"end", LEX_END}
    };

    void advance() { current_char = file.get(); }
    void skip_whitespace() { while (isspace(current_char)) advance(); }
    void skip_comment() {
        advance(); advance();
        while (current_char != EOF) {
            if (current_char == '*' && file.peek() == '/') {
                advance(); advance();
                return;
            }
            advance();
        }
    }

    Lex parse_identifier() {
        string result;
        while (isalnum(current_char)) {
            result += current_char;
            advance();
        }
        return keywords.count(result) ? Lex(keywords[result], result) : Lex(LEX_ID, result);
    }

    Lex parse_number() {
        string result;

        while (isalnum(current_char) || current_char == '.' || current_char == '+' || current_char == '-') {
            result += current_char;
            advance();
        }

        return isNum(result) ? Lex(LEX_NUM, result) : throw runtime_error("Invalid number " + result);
    }

    Lex parse_operator() {
        string result(1, current_char);
        advance();

        if (result == ":") {
            if (current_char == '=') {
                result += current_char;
                advance();
                return Lex(LEX_ASSIGN, result);
            }
            else {
                throw runtime_error("':' is used incorrectly.");
            }
        }

        if (result == "=") {
            if (current_char == '=') {
                result += current_char;
                advance();
                return Lex(LEX_EQ, result);
            }
            else {
                throw runtime_error("'=' is used incorrectly.");
            }
        }

        if (result == "!") {
            if (current_char == '=') {
                result += current_char;
                advance();
                return Lex(LEX_NEQ, result);
            }
            return Lex(LEX_NOT, result);
        }

        if (result == "<") {
            if (current_char == '=') {
                result += current_char;
                advance();
                return Lex(LEX_LEQ, result);
            }
            return Lex(LEX_LT, result);
        }

        if (result == ">") {
            if (current_char == '=') {
                result += current_char;
                advance();
                return Lex(LEX_GEQ, result);
            }
            return Lex(LEX_GT, result);
        }

        if (result == "|") {
            if (current_char == '|') {
                result += current_char;
                advance();
                return Lex(LEX_OR, result);
            }
            return Lex(LEX_DELIM, result);
        }

        if (result == "&") {
            if (current_char == '&') {
                result += current_char;
                advance();
                return Lex(LEX_AND, result);
            }
            return Lex(LEX_DELIM, result);
        }

        if (result == "+") return Lex(LEX_PLUS, result);
        if (result == "-") return Lex(LEX_MINUS, result);
        if (result == "*") return Lex(LEX_TIMES, result);
        if (result == "/") return Lex(LEX_DIV, result);

        return Lex(LEX_DELIM, result);
    }

    Lex parse_delimiter() {
        string result(1, current_char);
        advance();
        return Lex(LEX_DELIM, result);
    }

public:
    explicit Lexer(const string& filename) {
        file.open(filename);
        if (!file.is_open()) {
            throw runtime_error("File not found or cannot be opened");
        }
        advance();
    }

    vector<Lex> get_lex_table() {
        vector<Lex> lex_table;
        Lex token;
        while ((token = get_next_lex()).type != LEX_FIN) {
            lex_table.push_back(token);
        }
        return lex_table;
    }

    Lex get_next_lex() {
        while (current_char != EOF) {
            if (current_char == '/' && file.peek() == '*') {
                skip_comment();
                continue;
            }
            if (isspace(current_char)) {
                skip_whitespace();
                continue;
            }
            if (isalpha(current_char)) return parse_identifier();
            if (isdigit(current_char) || current_char == '.') return parse_number();
            if (current_char == '=' || current_char == '!' || current_char == '<' || current_char == '>' ||
                current_char == '+' || current_char == '-' || current_char == '*' || current_char == '/' ||
                current_char == '&' || current_char == '|' || current_char == ':') {
                return parse_operator();
            }
            return parse_delimiter();
        }
        return Lex(LEX_FIN, "");
    }
};

const char* lex_type_description(lex_type type) {
    switch (type) {
    case LEX_NULL: return "NULL";
    case LEX_DIM: return "DIM";
    case LEX_INTEGER: return "INTEGER";
    case LEX_REAL: return "REAL";
    case LEX_BOOLEAN: return "BOOLEAN";
    case LEX_IF: return "IF";
    case LEX_ELSE: return "ELSE";
    case LEX_FOR: return "FOR";
    case LEX_TO: return "TO";
    case LEX_STEP: return "STEP";
    case LEX_NEXT: return "NEXT";
    case LEX_WHILE: return "WHILE";
    case LEX_READLN: return "READLN";
    case LEX_WRITELN: return "WRITELN";
    case LEX_TRUE: return "TRUE";
    case LEX_FALSE: return "FALSE";
    case LEX_ID: return "IDENTIFIER";
    case LEX_NUM: return "NUMBER";
    case LEX_ASSIGN: return "ASSIGN";
    case LEX_EQ: return "EQUAL";
    case LEX_NEQ: return "NOT EQUAL";
    case LEX_LT: return "LESS THAN";
    case LEX_LEQ: return "LESS OR EQUAL";
    case LEX_GT: return "GREATER THAN";
    case LEX_GEQ: return "GREATER OR EQUAL";
    case LEX_PLUS: return "PLUS";
    case LEX_MINUS: return "MINUS";
    case LEX_OR: return "OR";
    case LEX_TIMES: return "TIMES";
    case LEX_DIV: return "DIVIDE";
    case LEX_AND: return "AND";
    case LEX_NOT: return "NOT";
    case LEX_BEGIN: return "BEGIN";
    case LEX_END: return "END";
    case LEX_FIN: return "FIN";
    case LEX_DELIM: return "DELIMITER";
    default: return "UNKNOWN";
    }
}

int main() {
    try {
        Lexer lexer("input.txt");
        vector<Lex> lex_table = lexer.get_lex_table();

        cout << "Lexical Table:" << endl;
        for (const auto& token : lex_table) {
            cout << "Token: " << lex_type_description(token.type)
                << ", value: " << token.value << endl;
        }
    }
    catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
    return 0;
}
