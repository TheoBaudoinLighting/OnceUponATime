// lexer.hpp

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "token.hpp"

class Lexer {
public:
    explicit Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string source;
    size_t pos;
    int line;
    int column;
    static const std::unordered_map<std::string, TokenType> keywordMap; 

    bool isAtEnd() const;
    char peek() const;
    char advance();
    void skipWhitespace();
    Token makeToken(TokenType type, const std::string& lexeme, int tokenStartColumn);
    Token readWord();
    Token readNumber();
    Token readString();
    std::string peekAhead() const;
};

#endif // LEXER_HPP
