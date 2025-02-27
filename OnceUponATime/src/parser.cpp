// parser.cpp

#include "parser.hpp"
#include <sstream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens)
    : tokens(tokens), current(0) {}

bool Parser::isAtEnd() const {
    return current >= tokens.size() || tokens[current].type == TokenType::END_OF_FILE;
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType expected) const {
    if (isAtEnd()) return false;
    return peek().type == expected;
}

bool Parser::match(TokenType expected) {
    if (check(expected)) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) return advance();
    std::ostringstream oss;
    oss << errorMessage << " at line " << peek().line << ", column " << peek().column;
    throw std::runtime_error(oss.str());
}

const Token& Parser::lookAhead(size_t offset) const {
    if (current + offset >= tokens.size()) {
        return tokens.back();
    }
    return tokens[current + offset];
}

static std::string toLower(const std::string &s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

bool Parser::checkEndMarker() const {
    if (current + 3 >= tokens.size()) {
        return false;
    }
    
    return toLower(tokens[current].lexeme) == "the" && 
           toLower(tokens[current + 1].lexeme) == "story" && 
           toLower(tokens[current + 2].lexeme) == "ends" && 
           tokens[current + 3].type == TokenType::PERIOD;
}

bool Parser::isKeyword(const std::string& word, const std::string& keyword) const {
    return toLower(word) == toLower(keyword);
}

std::vector<Sentence> Parser::parseScript() {
    std::vector<Sentence> sentences;
    
    Token token = advance();
    if (toLower(token.lexeme) != "once")
        throw std::runtime_error("Script must start with 'Once upon a time.'");
    
    token = advance();
    if (toLower(token.lexeme) != "upon")
        throw std::runtime_error("Script must start with 'Once upon a time.'");
    
    token = advance();
    if (toLower(token.lexeme) != "a")
        throw std::runtime_error("Script must start with 'Once upon a time.'");
    
    token = advance();
    if (toLower(token.lexeme) != "time")
        throw std::runtime_error("Script must start with 'Once upon a time.'");
    
    token = advance();
    if (token.type != TokenType::PERIOD)
        throw std::runtime_error("Script must start with 'Once upon a time.'");
    
    while (!isAtEnd() && !checkEndMarker()) {
        if (check(TokenType::KW_IF) || (peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "if"))) {
            sentences.push_back(parseConditionalBlock());
        } else {
            sentences.push_back(parseSentence());
        }
    }
    
    if (isAtEnd() || !checkEndMarker()) {
        throw std::runtime_error("Script must end with 'The story ends.'");
    }
    
    advance();
    advance();
    advance();
    advance();
    
    return sentences;
}

Sentence Parser::parseSentence() {
    if (check(TokenType::KW_CHOOSE))
        return parseInteractive();
    if (check(TokenType::KW_IF) || (peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "if")))
        return parseConditionalBlock();

    Sentence sentence;
    std::vector<Token> tokensInSentence;

    int startLine = peek().line;
    int startColumn = peek().column;

    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        tokensInSentence.push_back(advance());
    }

    if (check(TokenType::PERIOD)) {
        advance();
    } else {
        std::ostringstream oss;
        oss << "Expected '.' at the end of the sentence at line " 
            << peek().line << ", column " << peek().column;
        throw std::runtime_error(oss.str());
    }

    if (tokensInSentence.empty()) {
        std::ostringstream oss;
        oss << "Empty sentence encountered at line " << startLine 
            << ", column " << startColumn 
            << ". Make sure there are no consecutive periods.";
        throw std::runtime_error(oss.str());
    }

    size_t verbIndex = 0;
    bool verbFound = false;
    for (size_t i = 0; i < tokensInSentence.size(); i++) {
        std::string lower = toLower(tokensInSentence[i].lexeme);
        if (lower == "was" || lower == "is" || lower == "became" || 
            tokensInSentence[i].type == TokenType::VERB) {
            verbIndex = i;
            verbFound = true;
            break;
        }
    }

    if (!verbFound) {
        std::ostringstream oss;
        for (const auto &token : tokensInSentence) {
            oss << token.lexeme << " ";
        }
        sentence.subject = oss.str();
        if (!sentence.subject.empty() && sentence.subject.back() == ' ')
            sentence.subject.pop_back();
        return sentence;
    }

    for (size_t i = 0; i < verbIndex; i++) {
        if (!sentence.subject.empty())
            sentence.subject += " ";
        sentence.subject += tokensInSentence[i].lexeme;
    }
    sentence.verb = tokensInSentence[verbIndex].lexeme;

    if (verbIndex + 1 < tokensInSentence.size()) {
        if (verbIndex + 1 == tokensInSentence.size() - 1) {
            sentence.object = tokensInSentence[verbIndex + 1].lexeme;
        } else {
            for (size_t i = verbIndex + 1; i < tokensInSentence.size() - 1; i++) {
                sentence.adjectives.push_back(tokensInSentence[i].lexeme);
            }
            sentence.object = tokensInSentence.back().lexeme;
        }
    }

    bool isUncertain = false;
    std::string randomSubject;
    std::string state1, state2;

    for (size_t i = 0; i < tokensInSentence.size(); i++) {
        std::string lower = toLower(tokensInSentence[i].lexeme);
        if (lower == "uncertain" || lower == "randomly" || 
            (lower == "not" && i+2 < tokensInSentence.size() && 
             (toLower(tokensInSentence[i+1].lexeme) == "know" || 
              toLower(tokensInSentence[i+1].lexeme) == "knew"))) {
            isUncertain = true;
            
            for (size_t j = i+1; j < tokensInSentence.size(); j++) {
                if (toLower(tokensInSentence[j].lexeme) == "if" || 
                    toLower(tokensInSentence[j].lexeme) == "whether") {
                    if (j+1 < tokensInSentence.size()) {
                        if (toLower(tokensInSentence[j+1].lexeme) == "the" && j+2 < tokensInSentence.size()) {
                            randomSubject = tokensInSentence[j+2].lexeme;
                        } else {
                            randomSubject = tokensInSentence[j+1].lexeme;
                        }
                    }
                    break;
                }
            }
            
            for (size_t j = i+1; j < tokensInSentence.size(); j++) {
                if (j+2 < tokensInSentence.size() && toLower(tokensInSentence[j].lexeme) == "was") {
                    if (state1.empty()) {
                        state1 = tokensInSentence[j+1].lexeme;
                    }
                    for (size_t k = j+2; k < tokensInSentence.size(); k++) {
                        if (toLower(tokensInSentence[k].lexeme) == "or" && k+1 < tokensInSentence.size()) {
                            state2 = tokensInSentence[k+1].lexeme;
                            break;
                        }
                    }
                    break;
                }
            }
            
            break;
        }
    }

    if (isUncertain && !randomSubject.empty()) {
        sentence.isRandom = true;
        sentence.subject = randomSubject;
        
        if (!state1.empty() && state2.empty()) {
            state2 = "not " + state1;
        }
        
        sentence.randomStates = std::make_pair(state1, state2);
    }

    return sentence;
}


Sentence Parser::parseInteractive() {
    Sentence sentence;
    sentence.isInteractive = true;
    Token subjectToken = advance();
    sentence.subject = subjectToken.lexeme;
    consume(TokenType::KW_CHOOSE, "Expected 'choose' after the subject");
    std::ostringstream promptStream;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        promptStream << advance().lexeme << " ";
    }
    sentence.object = promptStream.str();
    if (!sentence.object.empty() && sentence.object.back() == ' ')
        sentence.object.pop_back();
    consume(TokenType::PERIOD, "Expected '.' at the end of the sentence");
    return sentence;
}

std::vector<Sentence> Parser::parseBlock() {
    std::vector<Sentence> block;
    while (!check(TokenType::KW_ELSE) && !check(TokenType::KW_ENDIF) &&
           !check(TokenType::KW_END) && !isAtEnd() &&
           !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "else")) &&
           !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "end")) &&
           !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "the"))) {
        if (check(TokenType::KW_IF) || (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "if")))
            block.push_back(parseConditionalBlock());
        else
            block.push_back(parseSentence());
    }
    return block;
}

Sentence Parser::parseConditionalBlock() {
    if (check(TokenType::KW_IF))
        advance();
    else if (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "if"))
        advance();
    else
        throw std::runtime_error("Expected 'if' to start condition");
    
    Sentence sentence;
    sentence.isConditional = true;
    
    std::string conditionStr;
    while (!isAtEnd() && 
           !check(TokenType::KW_THEN) && 
           !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "then"))) {
        conditionStr += advance().lexeme + " ";
    }
    if (!conditionStr.empty()) {
        conditionStr.pop_back();
    }
    sentence.condition = conditionStr;
    
    if (check(TokenType::KW_THEN))
        advance();
    else if (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "then"))
        advance();
    else
        throw std::runtime_error("Expected 'then' after condition");
    
    while (!isAtEnd() && 
           !check(TokenType::KW_ELSE) && 
           !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "else")) && 
           !check(TokenType::KW_END) && 
           !check(TokenType::KW_ENDIF) && 
           !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "end"))) {
        sentence.thenBranch.push_back(parseSentence());
    }
    
    if (check(TokenType::KW_ELSE) || (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "else"))) {
        advance();
        
        if (check(TokenType::KW_IF) || (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "if"))) {
            advance();
            
            Sentence elseIfSentence;
            elseIfSentence.isConditional = true;
            
            std::string elseIfCondStr;
            while (!isAtEnd() && 
                   !check(TokenType::KW_THEN) && 
                   !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "then"))) {
                elseIfCondStr += advance().lexeme + " ";
            }
            if (!elseIfCondStr.empty()) {
                elseIfCondStr.pop_back();
            }
            elseIfSentence.condition = elseIfCondStr;
            
            if (check(TokenType::KW_THEN))
                advance();
            else if (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "then"))
                advance();
            else
                throw std::runtime_error("Expected 'then' after 'else if' condition");
            
            while (!isAtEnd() && 
                   !check(TokenType::KW_ELSE) && 
                   !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "else")) && 
                   !check(TokenType::KW_END) && 
                   !check(TokenType::KW_ENDIF) && 
                   !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "end"))) {
                elseIfSentence.thenBranch.push_back(parseSentence());
            }
            
            sentence.elseBranch.push_back(elseIfSentence);
            
            if (check(TokenType::KW_ELSE) || (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "else"))) {
                advance();
                
                while (!isAtEnd() && 
                       !check(TokenType::KW_END) && 
                       !check(TokenType::KW_ENDIF) && 
                       !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "end"))) {
                    sentence.elseBranch.push_back(parseSentence());
                }
            }
        } else {
            while (!isAtEnd() && 
                   !check(TokenType::KW_END) && 
                   !check(TokenType::KW_ENDIF) && 
                   !(check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "end"))) {
                sentence.elseBranch.push_back(parseSentence());
            }
        }
    }
    
    if (check(TokenType::KW_END) || 
        check(TokenType::KW_ENDIF) || 
        (check(TokenType::IDENTIFIER) && isKeyword(peek().lexeme, "end"))) {
        advance();
        consume(TokenType::PERIOD, "Expected '.' after 'End'");
    } else {
        throw std::runtime_error("Expected 'end' to end the condition at line " + 
                                std::to_string(peek().line) + ", column " + 
                                std::to_string(peek().column));
    }
    
    return sentence;
}

std::vector<Sentence> Parser::parseNarrativeBlock() {
    std::vector<Sentence> block;
    while (!check(TokenType::KW_END_OF_STORY) &&
           !check(TokenType::KW_OTHERWISE) &&
           !check(TokenType::KW_END) && !isAtEnd()) {
        block.push_back(parseSentence());
    }
    return block;
}