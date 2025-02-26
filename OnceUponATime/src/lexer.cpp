/**
 * @file lexer.cpp
 * @author Théo Baudoin
 * @brief Lexical analyzer implementation for the Once Upon a Time (.ouat) language
 * @date 2025-02-26
 * 
 * This file implements the lexical analysis (tokenization) of .ouat source code.
 * It breaks down the input text into tokens that can be processed by the parser.
 */

#include "lexer.hpp"
#include <cctype>
#include <sstream>
#include <stdexcept>

// Dictionary of keywords including header and script ending tokens
const std::unordered_map<std::string, TokenType> Lexer::keywordMap = {
    {"once",     TokenType::KW_ONCE},
    {"upon",     TokenType::KW_UPON},
    {"a",        TokenType::KW_A},
    {"time",     TokenType::KW_TIME},
    {"go",       TokenType::VERB},
    {"talk",     TokenType::VERB}, 
    {"look",     TokenType::VERB},
    {"character",TokenType::NOUN},
    {"object",   TokenType::NOUN},
    {"story",    TokenType::NOUN},
    {"big",      TokenType::ADJECTIVE},
    {"small",    TokenType::ADJECTIVE},
    {"old",      TokenType::ADJECTIVE},
    {"new",      TokenType::ADJECTIVE},
    {"end",      TokenType::KW_END}
};

/**
 * @brief Constructs a new Lexer object
 * @param source The source code to tokenize
 */
Lexer::Lexer(const std::string& source)
    : source(source), pos(0), line(1), column(1) {}

/**
 * @brief Checks if we've reached the end of input
 * @return true if at end, false otherwise
 */
bool Lexer::isAtEnd() const {
    return pos >= source.size();
}

/**
 * @brief Peeks at the current character without consuming it
 * @return The current character
 */
char Lexer::peek() const {
    return source[pos];
}

/**
 * @brief Advances to the next character and updates line/column tracking
 * @return The consumed character
 */
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

/**
 * @brief Skips whitespace characters in the input
 * @note Also skips comments starting with '#' until end of line
 */
void Lexer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        // Skip whitespace
        if (std::isspace(c)) {
            advance();
        }
        // Skip comments starting with '#' until end of line
        else if (c == '#') {
            // Consume the '#'
            advance();
            // Advance until end of line
            while (!isAtEnd() && peek() != '\n') {
                advance();
            }
        } else {
            break;
        }
    }
}

/**
 * @brief Creates a new token with the given properties
 * @param type Token type
 * @param lexeme Token text
 * @param tokenStartColumn Starting column of the token
 * @return The constructed Token
 */
Token Lexer::makeToken(TokenType type, const std::string& lexeme, int tokenStartColumn) {
    return Token{ type, lexeme, line, tokenStartColumn };
}

/**
 * @brief Reads a word (identifier or keyword) from the input
 * @return The token representing the word
 */
Token Lexer::readWord() {
    int startColumn = column;
    size_t start = pos;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_')) {
        advance();
    }
    std::string word = source.substr(start, pos - start);
    // Check if the word is a keyword
    auto it = keywordMap.find(word);
    if (it != keywordMap.end()) {
        return makeToken(it->second, word, startColumn);
    }
    return makeToken(TokenType::IDENTIFIER, word, startColumn);
}

/**
 * @brief Tokenizes the entire input source
 * @return Vector of tokens
 * @throws std::runtime_error if invalid characters are encountered
 */
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd())
            break;
        int tokenStartColumn = column;
        char current = peek();
        if (std::isalpha(current)) {
            tokens.push_back(readWord());
        } else if (current == '.') {
            advance();
            tokens.push_back(makeToken(TokenType::PERIOD, ".", tokenStartColumn));
        } else {
            std::ostringstream oss;
            oss << "Unexpected character '" << current << "' at line " << line << ", column " << column;
            throw std::runtime_error(oss.str());
        }
    }
    tokens.push_back(Token{TokenType::END_OF_FILE, "", line, column});
    return tokens;
}
