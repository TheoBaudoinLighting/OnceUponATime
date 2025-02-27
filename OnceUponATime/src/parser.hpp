// parser.hpp

#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include "token.hpp"

struct Sentence {
    std::string subject;
    std::string verb;
    std::vector<std::string> adjectives;
    std::string object;
    bool isConditional = false;
    bool isInteractive = false;
    bool isRandom = false;
    std::string condition;
    std::vector<Sentence> thenBranch;
    std::vector<Sentence> elseBranch;
    std::pair<std::string, std::string> randomStates;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<Sentence> parseScript();

private:
    const std::vector<Token>& tokens;
    size_t current;
    
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType expected) const;
    bool match(TokenType expected);
    const Token& consume(TokenType type, const std::string& errorMessage);
    const Token& lookAhead(size_t offset) const;
    bool checkEndMarker() const;
    bool isKeyword(const std::string& word, const std::string& keyword) const;
    Sentence parseSentence();
    Sentence parseInteractive();
    std::vector<Sentence> parseBlock();
    Sentence parseConditionalBlock();
    std::vector<Sentence> parseNarrativeBlock();
};

#endif // PARSER_HPP