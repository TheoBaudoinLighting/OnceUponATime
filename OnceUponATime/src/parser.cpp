// parser.cpp
#include "parser.hpp"
#include <sstream>
#include <stdexcept>
#include <algorithm>

static std::string toLower(const std::string &s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

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

std::unique_ptr<AST::Story> Parser::parseStory() {
    if (!isKeyword(advance().lexeme, "once") ||
        !isKeyword(advance().lexeme, "upon") ||
        !isKeyword(advance().lexeme, "a") ||
        !isKeyword(advance().lexeme, "time")) {
        throw std::runtime_error("The script must start with 'Once upon a time.'");
    }
    consume(TokenType::PERIOD, "Expected end of sentence after 'Once upon a time'");
    auto story = std::make_unique<AST::Story>();
    while (!isAtEnd() && !checkEndMarker()) {
        auto stmt = parseStatement();
        if (stmt) {
            story->statements.push_back(std::move(stmt));
        }
    }
    if (!checkEndMarker()) {
        throw std::runtime_error("The script must end with 'The story ends.'");
    }
    advance();
    advance();
    advance();
    advance();
    return story;
}

std::unique_ptr<AST::Statement> Parser::parseStatement() {
    if (check(TokenType::KW_IF) || (peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "if"))) {
        return parseConditionalStatement();
    }
    if (check(TokenType::KW_CHOOSE)) {
        return parseInteractiveStatement();
    }
    if (check(TokenType::KW_RANDOM)) {
        return parseRandomStatement();
    }
    if (check(TokenType::KW_WHILE)) {
        return parseWhileStatement();
    }
    if (check(TokenType::KW_FOR)) {
        return parseForEachStatement();
    }
    if (check(TokenType::KW_DEFINE_FUNCTION)) {
        return parseFunctionDeclaration();
    }
    if (check(TokenType::KW_CALL)) {
        return parseFunctionCall();
    }
    if (check(TokenType::KW_RETURN)) {
        return parseReturnStatement();
    }
    if (check(TokenType::KW_REMARK) || check(TokenType::KW_NOTE) || check(TokenType::KW_COMMENT)) {
        return parseCommentStatement();
    }
    if (check(TokenType::KW_TELL)) {
        return parseTellStatement();
    }
    return parseNarrativeStatement();
}

std::unique_ptr<AST::Statement> Parser::parseNarrativeStatement() {
    std::vector<Token> tokensInSentence;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        tokensInSentence.push_back(advance());
    }
    consume(TokenType::PERIOD, "Expected '.' at the end of the sentence");
    bool isVarDecl = false;
    for (const auto &token : tokensInSentence) {
        std::string lower = toLower(token.lexeme);
        if ((lower == "has" || lower == "is") ) {
            for (const auto &t : tokensInSentence) {
                if (t.type == TokenType::NUMBER) {
                    isVarDecl = true;
                    break;
                }
            }
        }
        if (isVarDecl) break;
    }
    if (isVarDecl) {
        return parseVariableDeclarationBlock(tokensInSentence);
    }
    std::ostringstream oss;
    for (const auto &t : tokensInSentence) {
        oss << t.lexeme << " ";
    }
    std::string text = oss.str();
    if (!text.empty() && text.back() == ' ')
        text.pop_back();
    return std::make_unique<AST::NarrativeStatement>(text);
}

std::unique_ptr<AST::Statement> Parser::parseConditionalStatement() {
    if (check(TokenType::KW_IF))
        advance();
    else if (peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "if"))
        advance();
    else
        throw std::runtime_error("Expected 'if' to start a condition");
    std::ostringstream conditionStream;
    while (!isAtEnd() && !check(TokenType::KW_THEN) &&
           !(peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "then"))) {
        conditionStream << advance().lexeme << " ";
    }
    std::string condition = conditionStream.str();
    if (!condition.empty() && condition.back() == ' ')
        condition.pop_back();
    consume(TokenType::KW_THEN, "Expected 'then' after the condition");
    auto condStmt = std::make_unique<AST::ConditionalStatement>(condition);
    condStmt->thenBranch = parseBlock();
    while (check(TokenType::KW_ELSE)) {
        advance();
        if (check(TokenType::KW_IF) || (peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "if"))) {
            auto elseIfStmt = parseConditionalStatement();
            condStmt->elseBranch.push_back(std::move(elseIfStmt));
            break;
        } else {
            condStmt->elseBranch = parseBlock();
            break;
        }
    }
    if (check(TokenType::KW_END) || check(TokenType::KW_ENDIF) ||
        (peek().type == TokenType::IDENTIFIER && isKeyword(peek().lexeme, "end"))) {
        advance();
        consume(TokenType::PERIOD, "Expected '.' after 'end'");
    } else {
        throw std::runtime_error("Expected end of condition");
    }
    return condStmt;
}

std::unique_ptr<AST::Statement> Parser::parseInteractiveStatement() {
    advance();
    std::ostringstream promptStream;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        promptStream << advance().lexeme << " ";
    }
    std::string prompt = promptStream.str();
    if (!prompt.empty() && prompt.back() == ' ')
        prompt.pop_back();
    consume(TokenType::PERIOD, "Expected '.' at the end of the interactive instruction");
    return std::make_unique<AST::InteractiveStatement>(prompt);
}

std::unique_ptr<AST::Statement> Parser::parseRandomStatement() {
    advance();
    std::ostringstream subjectStream;
    while (!isAtEnd() && !isKeyword(peek().lexeme, "leans")) {
        subjectStream << advance().lexeme << " ";
    }
    std::string subject = subjectStream.str();
    if (!subject.empty() && subject.back() == ' ')
        subject.pop_back();
    if (isKeyword(peek().lexeme, "leans"))
        advance();
    else
        throw std::runtime_error("Expected 'leans' in random instruction");
    if (isKeyword(peek().lexeme, "towards"))
        advance();
    else
        throw std::runtime_error("Expected 'towards' in random instruction");
    std::ostringstream firstStateStream;
    while (!isAtEnd() && !isKeyword(peek().lexeme, "or")) {
        firstStateStream << advance().lexeme << " ";
    }
    std::string firstState = firstStateStream.str();
    if (!firstState.empty() && firstState.back() == ' ')
        firstState.pop_back();
    if (isKeyword(peek().lexeme, "or"))
        advance();
    else
        throw std::runtime_error("Expected 'or' in random instruction");
    std::ostringstream secondStateStream;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        secondStateStream << advance().lexeme << " ";
    }
    std::string secondState = secondStateStream.str();
    if (!secondState.empty() && secondState.back() == ' ')
        secondState.pop_back();
    consume(TokenType::PERIOD, "Expected '.' at the end of the random instruction");
    return std::make_unique<AST::RandomStatement>(subject, std::make_pair(firstState, secondState));
}

std::unique_ptr<AST::Statement> Parser::parseWhileStatement() {
    advance();
    std::ostringstream conditionStream;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        conditionStream << advance().lexeme << " ";
    }
    std::string condition = conditionStream.str();
    if (!condition.empty() && condition.back() == ' ')
        condition.pop_back();
    consume(TokenType::PERIOD, "Expected '.' after the while condition");
    auto whileStmt = std::make_unique<AST::WhileStatement>(condition);
    whileStmt->body = parseBlock();
    consume(TokenType::KW_ENDWHILE, "Expected 'endwhile' to close the while loop");
    consume(TokenType::PERIOD, "Expected '.' after 'endwhile'");
    return whileStmt;
}

std::unique_ptr<AST::Statement> Parser::parseForEachStatement() {
    advance();
    if (!check(TokenType::KW_EACH))
        throw std::runtime_error("Expected 'each' after 'for'");
    advance();
    std::string iterator = advance().lexeme;
    if (!isKeyword(peek().lexeme, "in"))
        throw std::runtime_error("Expected 'in' in the for each loop");
    consume(TokenType::KW_IN, "Expected 'in' in the for each loop");
    std::ostringstream collectionStream;
    while (!check(TokenType::KW_DO) && !isAtEnd()) {
        collectionStream << advance().lexeme << " ";
    }
    std::string collection = collectionStream.str();
    if (!collection.empty() && collection.back() == ' ')
        collection.pop_back();
    if (!collection.empty() && collection.front() == '[' && collection.back() == ']') {
        collection = collection.substr(1, collection.size() - 2);
        while (!collection.empty() && std::isspace(collection.front())) { collection.erase(collection.begin()); }
        while (!collection.empty() && std::isspace(collection.back())) { collection.pop_back(); }
    }
    consume(TokenType::KW_DO, "Expected 'do' in the for each loop");
    auto forEachStmt = std::make_unique<AST::ForEachStatement>(iterator, collection);
    forEachStmt->body = parseBlock();
    consume(TokenType::KW_ENDFOR, "Expected 'endfor' to close the for each loop");
    consume(TokenType::PERIOD, "Expected '.' after 'endfor'");
    return forEachStmt;
}

std::unique_ptr<AST::Statement> Parser::parseFunctionDeclaration() {
    advance();
    if (!isKeyword(peek().lexeme, "the"))
        throw std::runtime_error("Expected 'the' after 'define'");
    advance();
    if (!isKeyword(peek().lexeme, "function"))
        throw std::runtime_error("Expected 'function' after 'define the'");
    advance();
    std::string funcName = advance().lexeme;
    if (!isKeyword(peek().lexeme, "as"))
        throw std::runtime_error("Expected 'as' after the function name");
    advance();
    auto funcDecl = std::make_unique<AST::FunctionDeclaration>(funcName);
    funcDecl->body = parseBlock();
    consume(TokenType::KW_ENDFUNCTION, "Expected 'endfunction' to close the function");
    consume(TokenType::PERIOD, "Expected '.' after 'endfunction'");
    return funcDecl;
}

std::unique_ptr<AST::Statement> Parser::parseFunctionCall() {
    advance();
    std::string funcName = advance().lexeme;
    consume(TokenType::PERIOD, "Expected '.' after the function call");
    return std::make_unique<AST::FunctionCall>(funcName);
}

std::unique_ptr<AST::Statement> Parser::parseReturnStatement() {
    advance();
    consume(TokenType::PERIOD, "Expected '.' after 'return'");
    return std::make_unique<AST::ReturnStatement>();
}

std::unique_ptr<AST::Statement> Parser::parseCommentStatement() {
    std::string comment = advance().lexeme;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        comment += " " + advance().lexeme;
    }
    consume(TokenType::PERIOD, "Expected '.' at the end of the comment");
    return std::make_unique<AST::CommentStatement>(comment);
}

std::unique_ptr<AST::Statement> Parser::parseVariableDeclarationBlock(const std::vector<Token>& tokensInSentence) {
    size_t splitIndex = 0;
    for (size_t i = 0; i < tokensInSentence.size(); i++) {
        std::string lower = toLower(tokensInSentence[i].lexeme);
        if (lower == "has" || lower == "is") {
            splitIndex = i;
            break;
        }
    }
    std::ostringstream ownerStream;
    for (size_t i = 0; i < splitIndex; i++) {
        ownerStream << tokensInSentence[i].lexeme << " ";
    }
    std::string owner = ownerStream.str();
    if (!owner.empty() && owner.back() == ' ')
        owner.pop_back();
    std::vector<std::vector<Token>> declSegments;
    std::vector<Token> currentSegment;
    for (size_t i = splitIndex; i < tokensInSentence.size(); i++) {
        if (toLower(tokensInSentence[i].lexeme) == "and") {
            if (!currentSegment.empty()) {
                declSegments.push_back(currentSegment);
                currentSegment.clear();
            }
        } else {
            currentSegment.push_back(tokensInSentence[i]);
        }
    }
    if (!currentSegment.empty())
        declSegments.push_back(currentSegment);
    auto block = std::make_unique<AST::VariableDeclarationBlock>();
    for (const auto &segment : declSegments) {
        size_t idx = 0;
        if (idx < segment.size() && (toLower(segment[idx].lexeme) == "has" || toLower(segment[idx].lexeme) == "is"))
            idx++;
        if (idx < segment.size() && toLower(segment[idx].lexeme) == "a")
            idx++;
        if (idx >= segment.size())
            throw std::runtime_error("Expected variable name in declaration");
        std::string varName = segment[idx++].lexeme;
        if (idx < segment.size() && (toLower(segment[idx].lexeme) == "of" || toLower(segment[idx].lexeme) == "is"))
            idx++;
        else
            throw std::runtime_error("Expected 'of' or 'is' in variable declaration");
        if (idx >= segment.size())
            throw std::runtime_error("Expected value in variable declaration");
        std::string value = segment[idx].lexeme;
        block->declarations.push_back(std::make_unique<AST::VariableDeclaration>(owner, varName, value));
    }
    return block;
}

std::vector<std::unique_ptr<AST::Statement>> Parser::parseBlock() {
    std::vector<std::unique_ptr<AST::Statement>> block;
    while (!isAtEnd()) {
        std::string nextLex = toLower(peek().lexeme);
        if (check(TokenType::KW_ELSE) ||
            check(TokenType::KW_END) ||
            check(TokenType::KW_ENDIF) ||
            check(TokenType::KW_ENDWHILE) ||
            check(TokenType::KW_ENDFOR) ||
            check(TokenType::KW_ENDFUNCTION) ||
            nextLex == "else" ||
            nextLex == "end" ||
            nextLex == "endif" ||
            nextLex == "endwhile" ||
            nextLex == "endfor" ||
            nextLex == "endfunction") {
            break;
        }
        auto stmt = parseStatement();
        if (stmt)
            block.push_back(std::move(stmt));
    }
    return block;
}

std::unique_ptr<AST::Statement> Parser::parseTellStatement() {
    advance();
    
    if (check(TokenType::STRING)) {
        std::string message = advance().lexeme; 
        consume(TokenType::PERIOD, "Expected '.' at the end of the Tell statement");
        return std::make_unique<AST::TellStatement>(message);
    } else {
        throw std::runtime_error("Expected a string after 'Tell'");
    }
}
