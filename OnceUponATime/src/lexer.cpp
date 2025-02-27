// lexer.cpp

#include "lexer.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <algorithm>

const std::unordered_map<std::string, TokenType> Lexer::keywordMap = {
    // Adjectives
    {"big", TokenType::ADJECTIVE},
    {"new", TokenType::ADJECTIVE}, 
    {"old", TokenType::ADJECTIVE},
    {"small", TokenType::ADJECTIVE},

    // Keywords
    {"a", TokenType::KW_A},
    {"by", TokenType::KW_BY},
    {"choose", TokenType::KW_CHOOSE},
    {"during", TokenType::KW_DURING},
    {"else", TokenType::KW_ELSE},
    {"End", TokenType::KW_ENDIF},
    {"if", TokenType::KW_IF},
    {"increased", TokenType::KW_INCREASED},
    {"once", TokenType::KW_ONCE},
    {"otherwise", TokenType::KW_OTHERWISE},
    {"raised", TokenType::KW_INCREASED},
    {"random", TokenType::KW_RANDOM},
    {"randomly", TokenType::KW_RANDOM},
    {"result", TokenType::KW_TOTAL},
    {"then", TokenType::KW_THEN},
    {"time", TokenType::KW_TIME},
    {"total", TokenType::KW_TOTAL},
    {"uncertain", TokenType::KW_UNCERTAIN},
    {"upon", TokenType::KW_UPON},
    {"when", TokenType::KW_WHEN},
    {"while", TokenType::KW_WHEN},

    // Nouns
    {"character", TokenType::NOUN},
    {"object", TokenType::NOUN},
    {"story", TokenType::NOUN},

    // Verbs
    {"add", TokenType::VERB},
    {"display", TokenType::VERB},
    {"go", TokenType::VERB},
    {"knew", TokenType::VERB},
    {"know", TokenType::VERB},
    {"look", TokenType::VERB},
    {"subtract", TokenType::VERB},
    {"talk", TokenType::VERB}
};

Lexer::Lexer(const std::string& source)
    : source(source), pos(0), line(1), column(1) {}

bool Lexer::isAtEnd() const {
    return pos >= source.size();
}

char Lexer::peek() const {
    return source[pos];
}

char Lexer::advance() {
    char c = source[pos++];
    if (c == '\n') {
        ++line;
        column = 1;
    } else {
        ++column;
    }
    return c;
}

void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (std::isspace(c)) {
            advance();
        }
        else if (c == '#') {
            advance();
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
            if (!isAtEnd()) {
                advance();
            }
        } else {
            break;
        }
    }
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, int tokenStartColumn) {
    return Token{ type, lexeme, line, tokenStartColumn };
}

static bool isWordChar(char c) {
    return std::isalnum(c) || c == '_' || c == ',' || c == ';' || c == ':' || c == '?' || c == '!' || c == '-' || c == '\'';
}

Token Lexer::readWord() {
    int startColumn = column;
    std::string word;
    while (!isAtEnd() && !std::isspace(peek()) && peek() != '.' && peek() != '"') {
        if (!isWordChar(peek())) {
            break;
        }
        word += advance();
    }
    std::string lower = word;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    auto it = keywordMap.find(lower);
    if (it != keywordMap.end()) {
        return makeToken(it->second, word, startColumn);
    }
    return makeToken(TokenType::IDENTIFIER, word, startColumn);
}

Token Lexer::readNumber() {
    int startColumn = column;
    size_t start = pos;
    while (!isAtEnd() && std::isdigit(peek())) {
        advance();
    }
    std::string number = source.substr(start, pos - start);
    return makeToken(TokenType::NUMBER, number, startColumn);
}

Token Lexer::readString() {
    int startColumn = column;
    advance(); 
    size_t start = pos;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\' && pos + 1 < source.size()) {
            advance();
            advance();
        } else {
            advance();
        }
    }
    if (isAtEnd()) {
        throw std::runtime_error("Unterminated string at line " + std::to_string(line));
    }
    std::string str = source.substr(start, pos - start);
    advance();
    return makeToken(TokenType::STRING, str, startColumn);
}

std::string Lexer::peekAhead() const {
    size_t tempPos = pos;
    std::string result;
    while (tempPos < source.size() && std::isalnum(source[tempPos])) {
        result += source[tempPos++];
    }
    return result;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    skipWhitespace();
    while (!isAtEnd()) {
        int tokenStartColumn = column;
        char current = peek();
        if (!std::isprint(current)) {
            advance();
            continue;
        }
        if (current == '<') {
        }
        if (std::isdigit(current)) {
            tokens.push_back(readNumber());
        } else if (std::isalpha(current) || static_cast<unsigned char>(current) > 127) {
            tokens.push_back(readWord());
        } else if (current == '.') {
            advance();
            tokens.push_back(makeToken(TokenType::PERIOD, ".", tokenStartColumn));
        } else if (current == '"') {
            tokens.push_back(readString());
        } else {
            std::ostringstream oss;
            oss << "Unexpected character '" << current << "' at line " << line << ", column " << column;
            throw std::runtime_error(oss.str());
        }
        skipWhitespace();
    }
    tokens.push_back(Token{TokenType::END_OF_FILE, "", line, column});
    return tokens;
}
