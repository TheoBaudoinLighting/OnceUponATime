/**
 * @file parser.cpp
 * @author Théo Baudoin
 * @brief Parser implementation for the Once Upon a Time (.ouat) language
 * @version 1.0
 * @date 2025-02-26
 * 
 * @copyright Copyright (c) 2025
 */

#include "parser.hpp"
#include <sstream>
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), current(0) {}

/**
 * @brief Check if we've reached the end of input
 * @return true if at end, false otherwise
 */
bool Parser::isAtEnd() const {
    return current >= tokens.size() || tokens[current].type == TokenType::END_OF_FILE;
}

/**
 * @brief Get the current token without consuming it
 * @return const reference to current Token
 */
const Token& Parser::peek() const {
    return tokens[current];
}

/**
 * @brief Get the previous token
 * @return const reference to previous Token
 */
const Token& Parser::previous() const {
    return tokens[current - 1];
}

/**
 * @brief Advance to next token and return the previous one
 * @return const reference to previous Token
 */
const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

/**
 * @brief Check if current token matches expected type
 * @param expected The expected token type
 * @return true if matches, false otherwise
 */
bool Parser::check(TokenType expected) const {
    if (isAtEnd()) return false;
    return peek().type == expected;
}

/**
 * @brief Try to match and consume expected token type
 * @param expected The expected token type
 * @return true if matched and consumed, false otherwise
 */
bool Parser::match(TokenType expected) {
    if (check(expected)) {
        advance();
        return true;
    }
    return false;
}

/**
 * @brief Consume token if it matches expected type, otherwise throw error
 * @param type Expected token type
 * @param errorMessage Error message if token doesn't match
 * @return const reference to consumed Token
 * @throws std::runtime_error if token doesn't match expected type
 */
const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    std::ostringstream oss;
    oss << errorMessage << " at line " << peek().line << ", column " << peek().column;
    throw std::runtime_error(oss.str());
}

/**
 * @brief Parse a single sentence in the story
 * @return Sentence object containing parsed components
 * @throws std::runtime_error if syntax is invalid
 */
Sentence Parser::parseSentence() {
    Sentence sentence;
    // Accept subject as IDENTIFIER or NOUN
    if (check(TokenType::IDENTIFIER) || check(TokenType::NOUN)) {
        Token subjectToken = advance();
        sentence.subject = subjectToken.lexeme;
    } else {
        std::ostringstream oss;
        oss << "Expected subject (identifier or noun) at line " 
            << peek().line << ", column " << peek().column;
        throw std::runtime_error(oss.str());
    }
    
    // Expect a verb next
    Token verbToken = consume(TokenType::VERB, "Expected verb after subject");
    sentence.verb = verbToken.lexeme;
    
    // Optional adjectives
    while (check(TokenType::ADJECTIVE)) {
        Token adjToken = advance();
        sentence.adjectives.push_back(adjToken.lexeme);
    }
    
    // Optional object (IDENTIFIER or NOUN)
    if (check(TokenType::IDENTIFIER) || check(TokenType::NOUN)) {
        Token objectToken = advance();
        sentence.object = objectToken.lexeme;
    }
    
    // End of sentence: period required
    consume(TokenType::PERIOD, "Expected period '.' at end of sentence");
    return sentence;
}

/**
 * @brief Parse complete story script
 * @return Vector of parsed sentences
 * @throws std::runtime_error if script syntax is invalid
 */
std::vector<Sentence> Parser::parseScript() {
    std::vector<Sentence> sentences;
    // Check header: "once upon a time"
    consume(TokenType::KW_ONCE, "Expected 'once' at beginning of script");
    consume(TokenType::KW_UPON, "Expected 'upon' after 'once'");
    consume(TokenType::KW_A, "Expected 'a' after 'upon'");
    consume(TokenType::KW_TIME, "Expected 'time' after 'a'");
    
    // Parse sentences until we hit "end" token
    while (!isAtEnd() && peek().type != TokenType::KW_END) {
        sentences.push_back(parseSentence());
    }
    
    // Verify script ending: "end"
    consume(TokenType::KW_END, "Expected 'end' at end of script");
    
    return sentences;
}
