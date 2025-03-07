// lexer.cpp
#include "lexer.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <unordered_map>

const std::unordered_map<std::string, TokenType> Lexer::keywordMap = {
    {"once", TokenType::KW_ONCE},
    {"upon", TokenType::KW_UPON},
    {"a", TokenType::KW_A},
    {"time", TokenType::KW_TIME},
    {"big", TokenType::ADJECTIVE},
    {"new", TokenType::ADJECTIVE},
    {"old", TokenType::ADJECTIVE},
    {"small", TokenType::ADJECTIVE},
    {"by", TokenType::KW_BY},
    {"choose", TokenType::KW_CHOOSE},
    {"during", TokenType::KW_DURING},
    {"else", TokenType::KW_ELSE},
    {"end", TokenType::KW_END},
    {"if", TokenType::KW_IF},
    {"increased", TokenType::KW_INCREASED},
    {"otherwise", TokenType::KW_OTHERWISE},
    {"raised", TokenType::KW_INCREASED},
    {"random", TokenType::KW_RANDOM},
    {"randomly", TokenType::KW_RANDOM},
    {"result", TokenType::KW_TOTAL},
    {"then", TokenType::KW_THEN},
    {"total", TokenType::KW_TOTAL},
    {"uncertain", TokenType::KW_UNCERTAIN},
    {"when", TokenType::KW_WHEN},
    {"while", TokenType::KW_WHILE},
    {"endwhile", TokenType::KW_ENDWHILE},
    {"for", TokenType::KW_FOR},
    {"each", TokenType::KW_EACH},
    {"do", TokenType::KW_DO},
    {"endfor", TokenType::KW_ENDFOR},
    {"define", TokenType::KW_DEFINE_FUNCTION},
    {"endfunction", TokenType::KW_ENDFUNCTION},
    {"call", TokenType::KW_CALL},
    {"return", TokenType::KW_RETURN},
    {"has", TokenType::KW_HAS},
    {"is", TokenType::KW_IS},
    {"add", TokenType::KW_ADD},
    {"subtract", TokenType::KW_SUBTRACT},
    {"multiply", TokenType::KW_MULTIPLY},
    {"divide", TokenType::KW_DIVIDE},
    {"equals", TokenType::KW_EQUALS},
    {"input", TokenType::KW_INPUT},
    {"narrate", TokenType::KW_NARRATE},
    {"tell", TokenType::KW_TELL},
    {"remark:", TokenType::KW_REMARK},
    {"note:", TokenType::KW_NOTE},
    {"comment:", TokenType::KW_COMMENT},
    {"in", TokenType::KW_IN},
};

Lexer::Lexer(const std::string& source)
    : source(source), pos(0), line(1), column(1) {}

bool Lexer::isAtEnd() const {
    return pos >= source.size();
}

char Lexer::peek() const {
    assert(pos < source.size());
    return source[pos];
}

char Lexer::advance() {
    assert(pos < source.size());
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
        } else if (c == '#') {
            advance();
            while (!isAtEnd() && peek() != '\n') {
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
        if (std::isdigit(current)) {
            tokens.push_back(readNumber());
        } else if (std::isalpha(current) || static_cast<unsigned char>(current) > 127) {
            tokens.push_back(readWord());
        } else if (current == '.') {
            advance();
            tokens.push_back(makeToken(TokenType::PERIOD, ".", tokenStartColumn));
        } else if (current == '"') {
            tokens.push_back(readString());
        } else if (current == '[') {
            advance();
            tokens.push_back(makeToken(TokenType::LEFT_BRACKET, "[", tokenStartColumn));
        } else if (current == ']') {
            advance();
            tokens.push_back(makeToken(TokenType::RIGHT_BRACKET, "]", tokenStartColumn));
        } else if (current == ',') {  
            advance();
            tokens.push_back(makeToken(TokenType::COMMA, ",", tokenStartColumn));
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
