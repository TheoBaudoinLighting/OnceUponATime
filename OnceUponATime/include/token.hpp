/**
 * @file token.hpp
 * @author Théo Baudoin
 * @brief Token definitions for the Once Upon a Time (.ouat) language
 * @date 2025-02-26
 * 
 * This file defines the Token structure and TokenType enumeration
 * used by both the lexer and parser to represent language elements.
 */

#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

/**
 * @brief Enumeration of all possible token types in the .ouat language
 */
enum class TokenType {
    KW_ONCE,       // Keyword "once" for story beginning
    KW_UPON,       // Keyword "upon" for story beginning
    KW_A,          // Keyword "a" for story beginning
    KW_TIME,       // Keyword "time" for story beginning
    IDENTIFIER,    // Generic identifier for user-defined names
    VERB,          // Action keyword (go, talk, look)
    NOUN,          // Object keyword (character, object, story)
    ADJECTIVE,     // Descriptor keyword (big, small, old, new)
    PERIOD,        // Sentence terminator '.'
    KW_END,        // Keyword "end" for story ending
    END_OF_FILE,   // Marks end of input
};

/**
 * @brief Converts a TokenType to its string representation
 * @param type The token type to convert
 * @return std::string The string representation of the token type
 */
inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::KW_ONCE:      return "KW_ONCE";
        case TokenType::KW_UPON:      return "KW_UPON";
        case TokenType::KW_A:         return "KW_A";
        case TokenType::KW_TIME:      return "KW_TIME";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::VERB:         return "VERB";
        case TokenType::NOUN:         return "NOUN";
        case TokenType::ADJECTIVE:    return "ADJECTIVE";
        case TokenType::PERIOD:       return "PERIOD";
        case TokenType::KW_END:       return "KW_END";
        case TokenType::END_OF_FILE:  return "EOF";
        default:                      return "UNKNOWN";
    }
}

/**
 * @brief Structure representing a token with its type, text and position
 */
struct Token {
    TokenType type;      // Type of the token
    std::string lexeme;  // Actual text of the token
    int line;           // Line number in source
    int column;         // Column number in source
};

#endif // TOKEN_HPP
