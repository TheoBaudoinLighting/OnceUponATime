/**
 * @file parser.hpp
 * @author Théo Baudoin
 * @brief Parser header for the Once Upon a Time (.ouat) language
 * @date 2025-02-26
 * 
 * This file defines the Parser class which performs syntactic analysis
 * of .ouat source code. The parser processes tokens from the lexer and
 * builds an abstract syntax tree representing the story structure.
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <string>
#include "token.hpp"

/**
 * @brief Structure representing a sentence in the .ouat language
 */
struct Sentence {
    std::string subject;           // Subject of the sentence (noun or identifier)
    std::string verb;             // Action or state being described
    std::vector<std::string> adjectives;  // Optional descriptive words
    std::string object;           // Optional target of the action
};

/**
 * @brief Parser class for analyzing .ouat source code syntax
 */
class Parser {
public:
    /**
     * @brief Constructs a new Parser object
     * @param tokens Vector of tokens from lexical analysis
     */
    explicit Parser(const std::vector<Token>& tokens);

    /**
     * @brief Parses the complete script and returns list of sentences
     * @return Vector of parsed Sentence objects
     * @throws std::runtime_error if syntax is invalid
     */
    std::vector<Sentence> parseScript();

private:
    const std::vector<Token>& tokens;  // Input token stream
    size_t current;                    // Current position in token stream
    
    bool isAtEnd() const;              // Check if at end of input
    const Token& peek() const;         // Look at current token
    const Token& previous() const;     // Get previous token
    const Token& advance();            // Move to next token
    bool check(TokenType expected) const;  // Check if current token matches type
    bool match(TokenType expected);     // Try to match and consume token
    const Token& consume(TokenType type, const std::string& errorMessage);  // Consume expected token or error
    Sentence parseSentence();          // Parse a single sentence
};

#endif // PARSER_HPP
