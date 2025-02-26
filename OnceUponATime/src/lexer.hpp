/**
 * @file lexer.hpp
 * @author Théo Baudoin
 * @brief Lexer header for the Once Upon a Time (.ouat) language
 * @date 2025-02-26
 * 
 * This file defines the Lexer class which performs lexical analysis
 * of .ouat source code. The lexer breaks down the input text into
 * tokens that can be processed by the parser.
 */

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "token.hpp"

/**
 * @brief Lexical analyzer class for tokenizing .ouat source code
 */
class Lexer {
public:
    /**
     * @brief Constructs a new Lexer object
     * @param source The source code to tokenize
     */
    explicit Lexer(const std::string& source);

    /**
     * @brief Tokenizes the entire input source
     * @return Vector of tokens
     * @throws std::runtime_error if invalid characters are encountered
     */
    std::vector<Token> tokenize();

private:
    std::string source;                  // Input source code
    size_t pos;                          // Current position in source
    int line;                            // Current line number
    int column;                          // Current column number
    static const std::unordered_map<std::string, TokenType> keywordMap;  // Keyword lookup table

    bool isAtEnd() const;                // Check if at end of input
    char peek() const;                   // Look at current character
    char advance();                      // Move to next character
    void skipWhitespace();               // Skip whitespace characters
    Token makeToken(TokenType type, const std::string& lexeme, int tokenStartColumn);  // Create a new token
    Token readWord();                    // Read an identifier or keyword
};

#endif // LEXER_HPP
