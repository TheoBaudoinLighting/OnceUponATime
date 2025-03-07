// parser.hpp
#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include <memory>
#include "token.hpp"
#include "ast.hpp"

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<AST::Story> parseStory();
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
    std::unique_ptr<AST::Statement> parseStatement();
    std::unique_ptr<AST::Statement> parseNarrativeStatement();
    std::unique_ptr<AST::Statement> parseConditionalStatement();
    std::unique_ptr<AST::Statement> parseInteractiveStatement();
    std::unique_ptr<AST::Statement> parseRandomStatement();
    std::unique_ptr<AST::Statement> parseWhileStatement();
    std::unique_ptr<AST::Statement> parseForEachStatement();
    std::unique_ptr<AST::Statement> parseFunctionDeclaration();
    std::unique_ptr<AST::Statement> parseFunctionCall();
    std::unique_ptr<AST::Statement> parseReturnStatement();
    std::unique_ptr<AST::Statement> parseCommentStatement();
    std::unique_ptr<AST::Statement> parseVariableDeclarationBlock(const std::vector<Token>& tokensInSentence);
    std::unique_ptr<AST::Statement> parseTellStatement();
    std::vector<std::unique_ptr<AST::Statement>> parseBlock();
};

#endif
