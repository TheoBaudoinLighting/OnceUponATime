// parser.cpp
#include "parser.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

static std::string toLower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return res;
}

static bool sameWord(const Token& token, const std::string& word) {
    return toLower(token.lexeme) == toLower(word);
}

static bool isArticle(const Token& token) {
    std::string lower = toLower(token.lexeme);
    return lower == "a" || lower == "an" || lower == "the";
}

static bool isDeclarationSeparator(const Token& token) {
    std::string lower = toLower(token.lexeme);
    return lower == "of" || lower == "is";
}

static bool startsStatement(const Token& token) {
    switch (token.type) {
        case TokenType::KW_IF:
        case TokenType::KW_CHOOSE:
        case TokenType::KW_RANDOM:
        case TokenType::KW_WHILE:
        case TokenType::KW_FOR:
        case TokenType::KW_DEFINE_FUNCTION:
        case TokenType::KW_CALL:
        case TokenType::KW_RETURN:
        case TokenType::KW_REMARK:
        case TokenType::KW_NOTE:
        case TokenType::KW_COMMENT:
        case TokenType::KW_DISPLAY:
        case TokenType::KW_NARRATE:
        case TokenType::KW_TELL:
        case TokenType::KW_INPUT:
            return true;
        default:
            return false;
    }
}

static bool isArithmeticOperator(const Token& token) {
    return token.type == TokenType::KW_ADD ||
           token.type == TokenType::KW_SUBTRACT ||
           token.type == TokenType::KW_MULTIPLY ||
           token.type == TokenType::KW_DIVIDE;
}

static std::string joinTokens(const std::vector<Token>& tokens, size_t start = 0, size_t end = 0) {
    if (end == 0 || end > tokens.size()) {
        end = tokens.size();
    }

    std::ostringstream oss;
    bool needsSpace = false;
    for (size_t i = start; i < end; ++i) {
        const Token& token = tokens[i];
        if (token.type == TokenType::COMMA) {
            oss << ",";
            needsSpace = true;
            continue;
        }
        if (token.type == TokenType::RIGHT_BRACKET) {
            oss << "]";
            needsSpace = true;
            continue;
        }
        if (needsSpace && token.type != TokenType::LEFT_BRACKET) {
            oss << " ";
        }
        if (token.type == TokenType::LEFT_BRACKET) {
            oss << "[";
            needsSpace = false;
        } else {
            oss << token.lexeme;
            needsSpace = true;
        }
    }
    return oss.str();
}

static bool isArithmeticSentence(const std::vector<Token>& tokens) {
    bool hasOperator = false;
    bool hasEquals = false;
    for (const auto& token : tokens) {
        if (isArithmeticOperator(token)) {
            hasOperator = true;
        }
        if (token.type == TokenType::KW_EQUALS || sameWord(token, "equals")) {
            hasEquals = true;
        }
    }
    return hasOperator && hasEquals;
}

static bool isImageDeclarationSentence(const std::vector<Token>& tokens) {
    return tokens.size() >= 8 &&
           sameWord(tokens[0], "create") &&
           sameWord(tokens[1], "image");
}

static bool isPixelWriteSentence(const std::vector<Token>& tokens) {
    return tokens.size() >= 8 &&
           (sameWord(tokens[0], "paint") || sameWord(tokens[0], "set")) &&
           std::any_of(tokens.begin(), tokens.end(), [](const Token& token) { return sameWord(token, "at"); }) &&
           std::any_of(tokens.begin(), tokens.end(), [](const Token& token) { return sameWord(token, "with"); });
}

static bool isImageFillSentence(const std::vector<Token>& tokens) {
    return tokens.size() >= 5 &&
           sameWord(tokens[0], "fill") &&
           std::any_of(tokens.begin(), tokens.end(), [](const Token& token) { return sameWord(token, "with"); });
}

static bool isRectanglePaintSentence(const std::vector<Token>& tokens) {
    return tokens.size() >= 10 &&
           sameWord(tokens[0], "paint") &&
           sameWord(tokens[1], "rectangle") &&
           std::any_of(tokens.begin(), tokens.end(), [](const Token& token) { return sameWord(token, "from"); }) &&
           std::any_of(tokens.begin(), tokens.end(), [](const Token& token) { return sameWord(token, "to"); }) &&
           std::any_of(tokens.begin(), tokens.end(), [](const Token& token) { return sameWord(token, "with"); });
}

static bool isImageSaveSentence(const std::vector<Token>& tokens) {
    return tokens.size() >= 5 &&
           sameWord(tokens[0], "save") &&
           sameWord(tokens[1], "image");
}

static bool isRecordInstanceSentence(const std::vector<Token>& tokens) {
    size_t isIndex = tokens.size();
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::KW_IS || sameWord(tokens[i], "is")) {
            isIndex = i;
            break;
        }
    }
    if (isIndex == tokens.size() || isIndex == 0) {
        return false;
    }

    size_t typeIndex = isIndex + 1;
    if (typeIndex < tokens.size() && isArticle(tokens[typeIndex])) {
        typeIndex++;
    }
    if (typeIndex >= tokens.size()) {
        return false;
    }

    for (size_t i = typeIndex + 1; i < tokens.size(); ++i) {
        if (sameWord(tokens[i], "with")) {
            return i + 1 < tokens.size();
        }
    }
    return false;
}

static std::vector<std::vector<Token>> splitByAnd(const std::vector<Token>& tokens, size_t start = 0) {
    std::vector<std::vector<Token>> segments;
    std::vector<Token> current;
    int bracketDepth = 0;
    for (size_t i = start; i < tokens.size(); ++i) {
        if (tokens[i].type == TokenType::LEFT_BRACKET) {
            bracketDepth++;
        } else if (tokens[i].type == TokenType::RIGHT_BRACKET && bracketDepth > 0) {
            bracketDepth--;
        }

        if (bracketDepth == 0 && sameWord(tokens[i], "and")) {
            if (!current.empty()) {
                segments.push_back(current);
                current.clear();
            }
        } else {
            current.push_back(tokens[i]);
        }
    }
    if (!current.empty()) {
        segments.push_back(current);
    }
    return segments;
}

static std::vector<std::string> splitExpressionsByComma(const std::vector<Token>& tokens, size_t start, size_t end) {
    std::vector<std::string> expressions;
    std::vector<Token> current;
    for (size_t i = start; i < end; ++i) {
        if (tokens[i].type == TokenType::COMMA) {
            if (!current.empty()) {
                expressions.push_back(joinTokens(current));
                current.clear();
            }
        } else {
            current.push_back(tokens[i]);
        }
    }
    if (!current.empty()) {
        expressions.push_back(joinTokens(current));
    }
    return expressions;
}

static std::vector<std::string> parseColorExpressions(const std::vector<Token>& tokens, size_t start, size_t end) {
    auto expressions = splitExpressionsByComma(tokens, start, end);
    if (expressions.size() == 3) {
        return expressions;
    }

    if (end >= start && end - start == 3) {
        return {tokens[start].lexeme, tokens[start + 1].lexeme, tokens[start + 2].lexeme};
    }

    std::string colorName = joinTokens(tokens, start, end);
    if (colorName.empty()) {
        throw std::runtime_error("Expected a color name or three color expressions");
    }
    return {colorName + " red", colorName + " green", colorName + " blue"};
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
    consume(TokenType::KW_ONCE, "The script must start with 'Once upon a time.'");
    consume(TokenType::KW_UPON, "The script must start with 'Once upon a time.'");
    consume(TokenType::KW_A, "The script must start with 'Once upon a time.'");
    consume(TokenType::KW_TIME, "The script must start with 'Once upon a time.'");
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
    if (check(TokenType::KW_IF)) {
        return parseConditionalStatement();
    }
    if (check(TokenType::KW_CHOOSE) || check(TokenType::KW_INPUT)) {
        return parseInteractiveStatement();
    }
    if (check(TokenType::KW_RANDOM)) {
        return parseRandomStatement();
    }
    if (check(TokenType::KW_WHILE)) {
        return parseWhileStatement();
    }
    if (check(TokenType::KW_FOR)) {
        if (lookAhead(1).type == TokenType::KW_EACH &&
            lookAhead(3).type == TokenType::IDENTIFIER &&
            sameWord(lookAhead(3), "from")) {
            return parseForRangeStatement();
        }
        return parseForEachStatement();
    }
    if (check(TokenType::KW_DEFINE_FUNCTION)) {
        if ((lookAhead(1).type == TokenType::IDENTIFIER && sameWord(lookAhead(1), "record")) ||
            (lookAhead(1).type == TokenType::IDENTIFIER && sameWord(lookAhead(1), "the") &&
             lookAhead(2).type == TokenType::IDENTIFIER && sameWord(lookAhead(2), "record"))) {
            return parseRecordDeclaration();
        }
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
    if (check(TokenType::KW_DISPLAY) || check(TokenType::KW_NARRATE) || check(TokenType::KW_TELL)) {
        return parseOutputStatement();
    }
    return parseNarrativeStatement();
}

std::unique_ptr<AST::Statement> Parser::parseNarrativeStatement() {
    std::vector<Token> tokensInSentence;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        tokensInSentence.push_back(advance());
    }
    consume(TokenType::PERIOD, "Expected '.' at the end of the sentence");

    if (isRecordInstanceSentence(tokensInSentence)) {
        return parseRecordInstanceDeclaration(tokensInSentence);
    }
    if (isImageDeclarationSentence(tokensInSentence)) {
        return parseImageDeclaration(tokensInSentence);
    }
    if (isImageFillSentence(tokensInSentence)) {
        return parseImageFillStatement(tokensInSentence);
    }
    if (isRectanglePaintSentence(tokensInSentence)) {
        return parseRectanglePaintStatement(tokensInSentence);
    }
    if (isPixelWriteSentence(tokensInSentence)) {
        return parsePixelWriteStatement(tokensInSentence);
    }
    if (isImageSaveSentence(tokensInSentence)) {
        return parseImageSaveStatement(tokensInSentence);
    }
    if (isArithmeticSentence(tokensInSentence)) {
        return parseArithmeticStatement(tokensInSentence);
    }

    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (tokensInSentence[i].type == TokenType::KW_CHOOSE ||
            tokensInSentence[i].type == TokenType::KW_INPUT ||
            sameWord(tokensInSentence[i], "choose") ||
            sameWord(tokensInSentence[i], "input")) {
            for (size_t j = i + 1; j < tokensInSentence.size(); ++j) {
                if (tokensInSentence[j].type == TokenType::STRING) {
                    return std::make_unique<AST::InteractiveStatement>(tokensInSentence[j].lexeme);
                }
            }
            return std::make_unique<AST::InteractiveStatement>(joinTokens(tokensInSentence, i + 1));
        }
    }

    bool hasDeclarationMarker = false;
    bool startsWithArticle = !tokensInSentence.empty() && isArticle(tokensInSentence.front());
    bool hasLiteralValue = false;
    for (const auto& token : tokensInSentence) {
        if (token.type == TokenType::NUMBER || token.type == TokenType::STRING ||
            token.type == TokenType::LEFT_BRACKET) {
            hasLiteralValue = true;
            break;
        }
    }
    for (const auto& token : tokensInSentence) {
        if (token.type == TokenType::KW_HAS || sameWord(token, "has")) {
            hasDeclarationMarker = true;
            break;
        }
        if ((token.type == TokenType::KW_IS || sameWord(token, "is")) &&
            (startsWithArticle || hasLiteralValue)) {
            hasDeclarationMarker = true;
            break;
        }
    }

    if (hasDeclarationMarker) {
        return parseVariableDeclarationBlock(tokensInSentence);
    }

    return std::make_unique<AST::NarrativeStatement>(joinTokens(tokensInSentence));
}

std::unique_ptr<AST::Statement> Parser::parseConditionalStatement() {
    return parseConditionalStatement(true);
}

std::unique_ptr<AST::Statement> Parser::parseConditionalStatement(bool consumeEndMarker) {
    consume(TokenType::KW_IF, "Expected 'if' to start a condition");

    std::ostringstream conditionStream;
    while (!isAtEnd() && !check(TokenType::KW_THEN)) {
        conditionStream << advance().lexeme << " ";
    }
    std::string condition = conditionStream.str();
    if (!condition.empty() && condition.back() == ' ') {
        condition.pop_back();
    }
    consume(TokenType::KW_THEN, "Expected 'then' after the condition");

    auto condStmt = std::make_unique<AST::ConditionalStatement>(condition);
    condStmt->thenBranch = parseBlock();

    if (check(TokenType::KW_ELSE)) {
        advance();
        if (check(TokenType::KW_IF)) {
            condStmt->elseBranch.push_back(parseConditionalStatement(false));
        } else {
            condStmt->elseBranch = parseBlock();
        }
    }

    if (consumeEndMarker) {
        if (check(TokenType::KW_END) || check(TokenType::KW_ENDIF)) {
            advance();
            consume(TokenType::PERIOD, "Expected '.' after condition terminator");
        } else {
            throw std::runtime_error("Expected 'end.' or 'endif.' to close the condition");
        }
    }
    return condStmt;
}

std::unique_ptr<AST::Statement> Parser::parseInteractiveStatement() {
    advance();
    if (check(TokenType::STRING)) {
        std::string prompt = advance().lexeme;
        consume(TokenType::PERIOD, "Expected '.' at the end of the interactive instruction");
        return std::make_unique<AST::InteractiveStatement>(prompt);
    }

    std::ostringstream promptStream;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        promptStream << advance().lexeme << " ";
    }
    std::string prompt = promptStream.str();
    if (!prompt.empty() && prompt.back() == ' ') {
        prompt.pop_back();
    }
    consume(TokenType::PERIOD, "Expected '.' at the end of the interactive instruction");
    return std::make_unique<AST::InteractiveStatement>(prompt);
}

std::unique_ptr<AST::Statement> Parser::parseRandomStatement() {
    advance();
    std::ostringstream subjectStream;
    while (!isAtEnd() && !sameWord(peek(), "leans")) {
        subjectStream << advance().lexeme << " ";
    }
    std::string subject = subjectStream.str();
    if (!subject.empty() && subject.back() == ' ') {
        subject.pop_back();
    }
    if (isAtEnd() || !sameWord(peek(), "leans")) {
        throw std::runtime_error("Expected 'leans' in random instruction");
    }
    advance();
    if (isAtEnd() || !sameWord(peek(), "towards")) {
        throw std::runtime_error("Expected 'towards' in random instruction");
    }
    advance();

    std::ostringstream firstStateStream;
    while (!isAtEnd() && !sameWord(peek(), "or")) {
        firstStateStream << advance().lexeme << " ";
    }
    std::string firstState = firstStateStream.str();
    if (!firstState.empty() && firstState.back() == ' ') {
        firstState.pop_back();
    }
    if (isAtEnd() || !sameWord(peek(), "or")) {
        throw std::runtime_error("Expected 'or' in random instruction");
    }
    advance();

    std::ostringstream secondStateStream;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        secondStateStream << advance().lexeme << " ";
    }
    std::string secondState = secondStateStream.str();
    if (!secondState.empty() && secondState.back() == ' ') {
        secondState.pop_back();
    }
    consume(TokenType::PERIOD, "Expected '.' at the end of the random instruction");
    return std::make_unique<AST::RandomStatement>(subject, std::make_pair(firstState, secondState));
}

std::unique_ptr<AST::Statement> Parser::parseWhileStatement() {
    advance();
    std::ostringstream conditionStream;
    bool hasConditionToken = false;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        if (hasConditionToken && startsStatement(peek())) {
            break;
        }
        conditionStream << advance().lexeme << " ";
        hasConditionToken = true;
    }
    std::string condition = conditionStream.str();
    if (!condition.empty() && condition.back() == ' ') {
        condition.pop_back();
    }
    match(TokenType::PERIOD);

    auto whileStmt = std::make_unique<AST::WhileStatement>(condition);
    whileStmt->body = parseBlock();
    consume(TokenType::KW_ENDWHILE, "Expected 'endwhile' to close the while loop");
    consume(TokenType::PERIOD, "Expected '.' after 'endwhile'");
    return whileStmt;
}

std::unique_ptr<AST::Statement> Parser::parseForEachStatement() {
    advance();
    consume(TokenType::KW_EACH, "Expected 'each' after 'for'");
    std::string iterator = advance().lexeme;
    consume(TokenType::KW_IN, "Expected 'in' in the for each loop");

    std::ostringstream collectionStream;
    while (!check(TokenType::KW_DO) && !isAtEnd()) {
        collectionStream << advance().lexeme << " ";
    }
    std::string collection = collectionStream.str();
    if (!collection.empty() && collection.back() == ' ') {
        collection.pop_back();
    }
    consume(TokenType::KW_DO, "Expected 'do' in the for each loop");

    auto forEachStmt = std::make_unique<AST::ForEachStatement>(iterator, collection);
    forEachStmt->body = parseBlock();
    consume(TokenType::KW_ENDFOR, "Expected 'endfor' to close the for each loop");
    consume(TokenType::PERIOD, "Expected '.' after 'endfor'");
    return forEachStmt;
}

std::unique_ptr<AST::Statement> Parser::parseForRangeStatement() {
    advance();
    consume(TokenType::KW_EACH, "Expected 'each' after 'for'");
    std::string iterator = advance().lexeme;
    if (!sameWord(peek(), "from")) {
        throw std::runtime_error("Expected 'from' in the numeric for loop");
    }
    advance();

    std::ostringstream startStream;
    while (!isAtEnd() && !sameWord(peek(), "to")) {
        startStream << advance().lexeme << " ";
    }
    std::string start = startStream.str();
    if (!start.empty() && start.back() == ' ') {
        start.pop_back();
    }
    if (!sameWord(peek(), "to")) {
        throw std::runtime_error("Expected 'to' in the numeric for loop");
    }
    advance();

    std::ostringstream endStream;
    while (!check(TokenType::KW_DO) && !isAtEnd()) {
        endStream << advance().lexeme << " ";
    }
    std::string end = endStream.str();
    if (!end.empty() && end.back() == ' ') {
        end.pop_back();
    }
    consume(TokenType::KW_DO, "Expected 'do' in the numeric for loop");

    auto forRangeStmt = std::make_unique<AST::ForRangeStatement>(iterator, start, end);
    forRangeStmt->body = parseBlock();
    consume(TokenType::KW_ENDFOR, "Expected 'endfor' to close the numeric for loop");
    consume(TokenType::PERIOD, "Expected '.' after 'endfor'");
    return forRangeStmt;
}

std::unique_ptr<AST::Statement> Parser::parseFunctionDeclaration() {
    advance();
    if (!sameWord(peek(), "the")) {
        throw std::runtime_error("Expected 'the' after 'define'");
    }
    advance();
    if (!sameWord(peek(), "function")) {
        throw std::runtime_error("Expected 'function' after 'define the'");
    }
    advance();
    std::string funcName = advance().lexeme;
    if (!sameWord(peek(), "as")) {
        throw std::runtime_error("Expected 'as' after the function name");
    }
    advance();
    match(TokenType::PERIOD);

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
    match(TokenType::PERIOD);
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

std::unique_ptr<AST::Statement> Parser::parseRecordDeclaration() {
    advance();
    if (sameWord(peek(), "the")) {
        advance();
    }
    if (!sameWord(peek(), "record")) {
        throw std::runtime_error("Expected 'record' after 'define'");
    }
    advance();

    if (check(TokenType::PERIOD) || isAtEnd()) {
        throw std::runtime_error("Expected a record name");
    }
    std::string recordName = advance().lexeme;
    if (!sameWord(peek(), "with")) {
        throw std::runtime_error("Expected 'with' after the record name");
    }
    advance();

    std::vector<Token> fieldTokens;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        fieldTokens.push_back(advance());
    }
    consume(TokenType::PERIOD, "Expected '.' after the record declaration");

    std::vector<std::pair<std::string, std::string>> fields;
    for (const auto& segment : splitByAnd(fieldTokens)) {
        if (segment.size() < 2) {
            throw std::runtime_error("Expected record field form '<field> <type>'");
        }

        size_t separator = segment.size();
        for (size_t i = 0; i < segment.size(); ++i) {
            if (sameWord(segment[i], "of") || sameWord(segment[i], "is") || sameWord(segment[i], "as")) {
                separator = i;
                break;
            }
        }

        std::string fieldName;
        std::string fieldType;
        if (separator != segment.size()) {
            if (separator == 0 || separator + 1 >= segment.size()) {
                throw std::runtime_error("Expected record field form '<field> of <type>'");
            }
            fieldName = joinTokens(segment, 0, separator);
            fieldType = joinTokens(segment, separator + 1);
        } else {
            fieldName = joinTokens(segment, 0, segment.size() - 1);
            fieldType = segment.back().lexeme;
        }

        if (fieldName.empty() || fieldType.empty()) {
            throw std::runtime_error("Expected record field name and type");
        }
        fields.emplace_back(fieldName, fieldType);
    }

    return std::make_unique<AST::RecordDeclaration>(recordName, std::move(fields));
}

std::unique_ptr<AST::Statement> Parser::parseArithmeticStatement(const std::vector<Token>& tokensInSentence) {
    size_t operatorIndex = tokensInSentence.size();
    size_t equalsIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (operatorIndex == tokensInSentence.size() && isArithmeticOperator(tokensInSentence[i])) {
            operatorIndex = i;
        }
        if (tokensInSentence[i].type == TokenType::KW_EQUALS || sameWord(tokensInSentence[i], "equals")) {
            equalsIndex = i;
            break;
        }
    }

    if (operatorIndex == tokensInSentence.size() || equalsIndex == tokensInSentence.size() ||
        operatorIndex == 0 || operatorIndex + 1 >= equalsIndex || equalsIndex + 1 >= tokensInSentence.size()) {
        throw std::runtime_error("Expected arithmetic form '<left> <operation> <right> equals <target>'");
    }

    std::string operation = toLower(tokensInSentence[operatorIndex].lexeme);
    return std::make_unique<AST::ArithmeticStatement>(
        joinTokens(tokensInSentence, 0, operatorIndex),
        operation,
        joinTokens(tokensInSentence, operatorIndex + 1, equalsIndex),
        joinTokens(tokensInSentence, equalsIndex + 1));
}

std::unique_ptr<AST::Statement> Parser::parseRecordInstanceDeclaration(const std::vector<Token>& tokensInSentence) {
    size_t isIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (tokensInSentence[i].type == TokenType::KW_IS || sameWord(tokensInSentence[i], "is")) {
            isIndex = i;
            break;
        }
    }

    size_t typeIndex = isIndex + 1;
    if (typeIndex < tokensInSentence.size() && isArticle(tokensInSentence[typeIndex])) {
        typeIndex++;
    }
    size_t withIndex = tokensInSentence.size();
    for (size_t i = typeIndex + 1; i < tokensInSentence.size(); ++i) {
        if (sameWord(tokensInSentence[i], "with")) {
            withIndex = i;
            break;
        }
    }

    if (isIndex == 0 || typeIndex >= tokensInSentence.size() || withIndex == tokensInSentence.size()) {
        throw std::runtime_error("Expected record instance form '<name> is a <Record> with <field> <value>'");
    }

    std::vector<std::pair<std::string, std::string>> fieldValues;
    for (const auto& segment : splitByAnd(tokensInSentence, withIndex + 1)) {
        if (segment.size() < 2) {
            throw std::runtime_error("Expected record value form '<field> <value>'");
        }
        size_t valueStart = 1;
        if (segment.size() >= 3 && (sameWord(segment[1], "of") || sameWord(segment[1], "is"))) {
            valueStart = 2;
        }
        std::string fieldName = segment[0].lexeme;
        std::string value = joinTokens(segment, valueStart);
        if (fieldName.empty() || value.empty()) {
            throw std::runtime_error("Expected record field name and value");
        }
        fieldValues.emplace_back(fieldName, value);
    }

    return std::make_unique<AST::RecordInstanceDeclaration>(
        joinTokens(tokensInSentence, 0, isIndex),
        tokensInSentence[typeIndex].lexeme,
        std::move(fieldValues));
}

std::unique_ptr<AST::Statement> Parser::parseImageDeclaration(const std::vector<Token>& tokensInSentence) {
    if (tokensInSentence.size() < 9) {
        throw std::runtime_error("Expected image form 'Create image <name> with width <width> and height <height>'");
    }

    size_t widthIndex = tokensInSentence.size();
    size_t andIndex = tokensInSentence.size();
    size_t heightIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (widthIndex == tokensInSentence.size() && sameWord(tokensInSentence[i], "width")) {
            widthIndex = i;
        } else if (widthIndex != tokensInSentence.size() && andIndex == tokensInSentence.size() &&
                   sameWord(tokensInSentence[i], "and")) {
            andIndex = i;
        } else if (andIndex != tokensInSentence.size() && sameWord(tokensInSentence[i], "height")) {
            heightIndex = i;
            break;
        }
    }

    if (widthIndex == tokensInSentence.size() || andIndex == tokensInSentence.size() ||
        heightIndex == tokensInSentence.size() || widthIndex + 1 >= andIndex ||
        heightIndex + 1 >= tokensInSentence.size()) {
        throw std::runtime_error("Expected image form 'Create image <name> with width <width> and height <height>'");
    }

    return std::make_unique<AST::ImageDeclaration>(
        tokensInSentence[2].lexeme,
        joinTokens(tokensInSentence, widthIndex + 1, andIndex),
        joinTokens(tokensInSentence, heightIndex + 1));
}

std::unique_ptr<AST::Statement> Parser::parsePixelWriteStatement(const std::vector<Token>& tokensInSentence) {
    size_t atIndex = tokensInSentence.size();
    size_t withIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (atIndex == tokensInSentence.size() && sameWord(tokensInSentence[i], "at")) {
            atIndex = i;
        } else if (atIndex != tokensInSentence.size() && sameWord(tokensInSentence[i], "with")) {
            withIndex = i;
            break;
        }
    }

    if (atIndex == tokensInSentence.size() || withIndex == tokensInSentence.size() ||
        atIndex + 2 >= withIndex || withIndex + 3 >= tokensInSentence.size()) {
        throw std::runtime_error("Expected pixel form 'Paint <image> at <x> <y> with <red> <green> <blue>'");
    }

    auto colorExpressions = parseColorExpressions(tokensInSentence, withIndex + 1, tokensInSentence.size());

    return std::make_unique<AST::PixelWriteStatement>(
        tokensInSentence[1].lexeme,
        tokensInSentence[atIndex + 1].lexeme,
        tokensInSentence[atIndex + 2].lexeme,
        colorExpressions[0],
        colorExpressions[1],
        colorExpressions[2]);
}

std::unique_ptr<AST::Statement> Parser::parseImageFillStatement(const std::vector<Token>& tokensInSentence) {
    size_t withIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (sameWord(tokensInSentence[i], "with")) {
            withIndex = i;
            break;
        }
    }

    size_t imageIndex = 1;
    if (tokensInSentence.size() > 2 && sameWord(tokensInSentence[1], "image")) {
        imageIndex = 2;
    }

    if (withIndex == tokensInSentence.size() || imageIndex >= withIndex || withIndex + 1 >= tokensInSentence.size()) {
        throw std::runtime_error("Expected fill form 'Fill image <name> with <color>'");
    }

    auto colorExpressions = parseColorExpressions(tokensInSentence, withIndex + 1, tokensInSentence.size());
    return std::make_unique<AST::ImageFillStatement>(
        tokensInSentence[imageIndex].lexeme,
        colorExpressions[0],
        colorExpressions[1],
        colorExpressions[2]);
}

std::unique_ptr<AST::Statement> Parser::parseRectanglePaintStatement(const std::vector<Token>& tokensInSentence) {
    size_t onIndex = tokensInSentence.size();
    size_t fromIndex = tokensInSentence.size();
    size_t toIndex = tokensInSentence.size();
    size_t withIndex = tokensInSentence.size();

    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (onIndex == tokensInSentence.size() && sameWord(tokensInSentence[i], "on")) {
            onIndex = i;
        } else if (fromIndex == tokensInSentence.size() && sameWord(tokensInSentence[i], "from")) {
            fromIndex = i;
        } else if (fromIndex != tokensInSentence.size() && toIndex == tokensInSentence.size() &&
                   sameWord(tokensInSentence[i], "to")) {
            toIndex = i;
        } else if (toIndex != tokensInSentence.size() && sameWord(tokensInSentence[i], "with")) {
            withIndex = i;
            break;
        }
    }

    if (onIndex == tokensInSentence.size() || fromIndex == tokensInSentence.size() ||
        toIndex == tokensInSentence.size() || withIndex == tokensInSentence.size() ||
        onIndex + 1 >= fromIndex || fromIndex + 2 >= toIndex ||
        toIndex + 2 >= withIndex || withIndex + 1 >= tokensInSentence.size()) {
        throw std::runtime_error("Expected rectangle form 'Paint rectangle on <image> from <left> <bottom> to <right> <top> with <color>'");
    }

    auto colorExpressions = parseColorExpressions(tokensInSentence, withIndex + 1, tokensInSentence.size());
    return std::make_unique<AST::RectanglePaintStatement>(
        tokensInSentence[onIndex + 1].lexeme,
        tokensInSentence[fromIndex + 1].lexeme,
        tokensInSentence[fromIndex + 2].lexeme,
        tokensInSentence[toIndex + 1].lexeme,
        tokensInSentence[toIndex + 2].lexeme,
        colorExpressions[0],
        colorExpressions[1],
        colorExpressions[2]);
}

std::unique_ptr<AST::Statement> Parser::parseImageSaveStatement(const std::vector<Token>& tokensInSentence) {
    size_t toIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); ++i) {
        if (sameWord(tokensInSentence[i], "to")) {
            toIndex = i;
            break;
        }
    }

    if (toIndex == tokensInSentence.size() || toIndex + 1 >= tokensInSentence.size() ||
        tokensInSentence[toIndex + 1].type != TokenType::STRING) {
        throw std::runtime_error("Expected image save form 'Save image <name> to \"path.ppm\"'");
    }

    return std::make_unique<AST::ImageSaveStatement>(
        tokensInSentence[2].lexeme,
        tokensInSentence[toIndex + 1].lexeme);
}

std::unique_ptr<AST::Statement> Parser::parseVariableDeclarationBlock(const std::vector<Token>& tokensInSentence) {
    size_t splitIndex = tokensInSentence.size();
    for (size_t i = 0; i < tokensInSentence.size(); i++) {
        if (tokensInSentence[i].type == TokenType::KW_HAS ||
            tokensInSentence[i].type == TokenType::KW_IS ||
            sameWord(tokensInSentence[i], "has") ||
            sameWord(tokensInSentence[i], "is")) {
            splitIndex = i;
            break;
        }
    }
    if (splitIndex == tokensInSentence.size()) {
        throw std::runtime_error("Expected 'has' or 'is' in variable declaration");
    }

    std::string owner = joinTokens(tokensInSentence, 0, splitIndex);
    std::vector<std::vector<Token>> declSegments;
    std::vector<Token> currentSegment;
    int bracketDepth = 0;
    bool startsWithIs = sameWord(tokensInSentence[splitIndex], "is");

    for (size_t i = splitIndex; i < tokensInSentence.size(); i++) {
        if (tokensInSentence[i].type == TokenType::LEFT_BRACKET) {
            bracketDepth++;
        } else if (tokensInSentence[i].type == TokenType::RIGHT_BRACKET && bracketDepth > 0) {
            bracketDepth--;
        }

        if (!startsWithIs && bracketDepth == 0 && sameWord(tokensInSentence[i], "and")) {
            if (!currentSegment.empty()) {
                declSegments.push_back(currentSegment);
                currentSegment.clear();
            }
        } else {
            currentSegment.push_back(tokensInSentence[i]);
        }
    }
    if (!currentSegment.empty()) {
        declSegments.push_back(currentSegment);
    }

    auto block = std::make_unique<AST::VariableDeclarationBlock>();
    for (const auto& segment : declSegments) {
        size_t idx = 0;
        bool stateAssignment = false;
        if (idx < segment.size() && sameWord(segment[idx], "has")) {
            idx++;
        } else if (idx < segment.size() && sameWord(segment[idx], "is")) {
            stateAssignment = true;
            idx++;
        }

        std::string varName;
        if (stateAssignment) {
            varName = "state";
        } else {
            if (idx < segment.size() && isArticle(segment[idx])) {
                idx++;
            }
            size_t nameStart = idx;
            while (idx < segment.size() &&
                   !isDeclarationSeparator(segment[idx]) &&
                   segment[idx].type != TokenType::LEFT_BRACKET) {
                idx++;
            }
            varName = joinTokens(segment, nameStart, idx);
            if (varName.empty()) {
                throw std::runtime_error("Expected variable name in declaration");
            }
            if (idx < segment.size() && isDeclarationSeparator(segment[idx])) {
                idx++;
            }
        }

        if (idx >= segment.size()) {
            throw std::runtime_error("Expected value in variable declaration");
        }

        if (segment[idx].type == TokenType::LEFT_BRACKET) {
            idx++;
            std::vector<std::string> values;
            while (idx < segment.size() && segment[idx].type != TokenType::RIGHT_BRACKET) {
                if (segment[idx].type != TokenType::COMMA) {
                    values.push_back(segment[idx].lexeme);
                }
                idx++;
            }
            block->declarations.push_back(
                std::make_unique<AST::VariableDeclaration>(owner, varName, std::move(values)));
        } else {
            std::string value = joinTokens(segment, idx);
            if (value.empty()) {
                throw std::runtime_error("Expected value in variable declaration");
            }
            block->declarations.push_back(
                std::make_unique<AST::VariableDeclaration>(owner, varName, value));
        }
    }
    return block;
}

std::vector<std::unique_ptr<AST::Statement>> Parser::parseBlock() {
    std::vector<std::unique_ptr<AST::Statement>> block;
    while (!isAtEnd() && !checkEndMarker()) {
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
        if (stmt) {
            block.push_back(std::move(stmt));
        }
    }
    return block;
}

std::unique_ptr<AST::Statement> Parser::parseOutputStatement() {
    advance();
    if (check(TokenType::STRING)) {
        std::string message = advance().lexeme;
        consume(TokenType::PERIOD, "Expected '.' at the end of the output statement");
        return std::make_unique<AST::TellStatement>(message);
    }

    std::vector<Token> messageTokens;
    while (!check(TokenType::PERIOD) && !isAtEnd()) {
        messageTokens.push_back(advance());
    }
    consume(TokenType::PERIOD, "Expected '.' at the end of the output statement");
    return std::make_unique<AST::TellStatement>(joinTokens(messageTokens));
}
