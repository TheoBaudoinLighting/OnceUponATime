// token_tests.cpp

#include <gtest/gtest.h>
#include "../include/token.hpp"
#include <string>
#include <vector>
#include <map>

TEST(TokenTest, TokenCreationTest) {
    Token token = {TokenType::IDENTIFIER, "variable", 10, 5};
    
    EXPECT_EQ(token.type, TokenType::IDENTIFIER);
    EXPECT_EQ(token.lexeme, "variable");
    EXPECT_EQ(token.line, 10);
    EXPECT_EQ(token.column, 5);
}

TEST(TokenTest, TokenTypeToStringTest) {
    // Test de conversion des types de tokens en chaînes
    EXPECT_EQ(tokenTypeToString(TokenType::KW_ONCE), "KW_ONCE");
    EXPECT_EQ(tokenTypeToString(TokenType::KW_UPON), "KW_UPON");
    EXPECT_EQ(tokenTypeToString(TokenType::KW_A), "KW_A");
    EXPECT_EQ(tokenTypeToString(TokenType::KW_TIME), "KW_TIME");
    EXPECT_EQ(tokenTypeToString(TokenType::IDENTIFIER), "IDENTIFIER");
    EXPECT_EQ(tokenTypeToString(TokenType::NUMBER), "NUMBER");
    EXPECT_EQ(tokenTypeToString(TokenType::STRING), "STRING");
    EXPECT_EQ(tokenTypeToString(TokenType::PERIOD), "PERIOD");
    EXPECT_EQ(tokenTypeToString(TokenType::END_OF_FILE), "END_OF_FILE");
}

TEST(TokenTest, KeywordTokensTest) {
    // Test des tokens de mots-clés
    Token once = {TokenType::KW_ONCE, "Once", 1, 1};
    Token upon = {TokenType::KW_UPON, "upon", 1, 6};
    Token a = {TokenType::KW_A, "a", 1, 11};
    Token time = {TokenType::KW_TIME, "time", 1, 13};
    
    EXPECT_EQ(once.type, TokenType::KW_ONCE);
    EXPECT_EQ(upon.type, TokenType::KW_UPON);
    EXPECT_EQ(a.type, TokenType::KW_A);
    EXPECT_EQ(time.type, TokenType::KW_TIME);
    
    EXPECT_EQ(once.lexeme, "Once");
    EXPECT_EQ(upon.lexeme, "upon");
    EXPECT_EQ(a.lexeme, "a");
    EXPECT_EQ(time.lexeme, "time");
}

TEST(TokenTest, OperatorTokensTest) {
    // Test des tokens d'opérateurs
    Token add = {TokenType::KW_ADD, "add", 5, 10};
    Token subtract = {TokenType::KW_SUBTRACT, "subtract", 6, 10};
    Token multiply = {TokenType::KW_MULTIPLY, "multiply", 7, 10};
    Token divide = {TokenType::KW_DIVIDE, "divide", 8, 10};
    Token equals = {TokenType::KW_EQUALS, "equals", 9, 10};
    
    EXPECT_EQ(add.type, TokenType::KW_ADD);
    EXPECT_EQ(subtract.type, TokenType::KW_SUBTRACT);
    EXPECT_EQ(multiply.type, TokenType::KW_MULTIPLY);
    EXPECT_EQ(divide.type, TokenType::KW_DIVIDE);
    EXPECT_EQ(equals.type, TokenType::KW_EQUALS);
}

TEST(TokenTest, ControlFlowTokensTest) {
    // Test des tokens de contrôle de flux
    Token if_token = {TokenType::KW_IF, "if", 10, 5};
    Token then = {TokenType::KW_THEN, "then", 10, 8};
    Token else_token = {TokenType::KW_ELSE, "else", 12, 5};
    Token endif = {TokenType::KW_ENDIF, "endif", 14, 5};
    Token while_token = {TokenType::KW_WHILE, "while", 16, 5};
    Token endwhile = {TokenType::KW_ENDWHILE, "endwhile", 18, 5};
    
    EXPECT_EQ(if_token.type, TokenType::KW_IF);
    EXPECT_EQ(then.type, TokenType::KW_THEN);
    EXPECT_EQ(else_token.type, TokenType::KW_ELSE);
    EXPECT_EQ(endif.type, TokenType::KW_ENDIF);
    EXPECT_EQ(while_token.type, TokenType::KW_WHILE);
    EXPECT_EQ(endwhile.type, TokenType::KW_ENDWHILE);
}

TEST(TokenTest, LiteralTokensTest) {
    // Test des tokens littéraux
    Token number = {TokenType::NUMBER, "42", 20, 5};
    Token string = {TokenType::STRING, "Hello, world!", 21, 5};
    Token identifier = {TokenType::IDENTIFIER, "princess", 22, 5};
    
    EXPECT_EQ(number.type, TokenType::NUMBER);
    EXPECT_EQ(string.type, TokenType::STRING);
    EXPECT_EQ(identifier.type, TokenType::IDENTIFIER);
    
    EXPECT_EQ(number.lexeme, "42");
    EXPECT_EQ(string.lexeme, "Hello, world!");
    EXPECT_EQ(identifier.lexeme, "princess");
}

TEST(TokenTest, PositionTrackingTest) {
    // Test du suivi de position (ligne, colonne)
    Token token1 = {TokenType::IDENTIFIER, "var1", 1, 5};
    Token token2 = {TokenType::IDENTIFIER, "var2", 2, 10};
    Token token3 = {TokenType::IDENTIFIER, "var3", 3, 15};
    
    EXPECT_EQ(token1.line, 1);
    EXPECT_EQ(token1.column, 5);
    
    EXPECT_EQ(token2.line, 2);
    EXPECT_EQ(token2.column, 10);
    
    EXPECT_EQ(token3.line, 3);
    EXPECT_EQ(token3.column, 15);
}

TEST(TokenTest, TokenSequenceTest) {
    // Test d'une séquence de tokens représentant un programme simple
    std::vector<Token> tokens = {
        {TokenType::KW_ONCE, "Once", 1, 1},
        {TokenType::KW_UPON, "upon", 1, 6},
        {TokenType::KW_A, "a", 1, 11},
        {TokenType::KW_TIME, "time", 1, 13},
        {TokenType::PERIOD, ".", 1, 17},
        {TokenType::IDENTIFIER, "princess", 2, 1},
        {TokenType::KW_HAS, "has", 2, 10},
        {TokenType::NUMBER, "42", 2, 14},
        {TokenType::NOUN, "apples", 2, 17},
        {TokenType::PERIOD, ".", 2, 23},
        {TokenType::KW_END_OF_STORY, "End of story", 3, 1},
        {TokenType::PERIOD, ".", 3, 13},
        {TokenType::END_OF_FILE, "", 3, 14}
    };
    
    // Vérification de la séquence
    EXPECT_EQ(tokens.size(), 13);
    EXPECT_EQ(tokens[0].type, TokenType::KW_ONCE);
    EXPECT_EQ(tokens[4].type, TokenType::PERIOD);
    EXPECT_EQ(tokens[5].lexeme, "princess");
    EXPECT_EQ(tokens[7].lexeme, "42");
    EXPECT_EQ(tokens[11].type, TokenType::PERIOD);
    EXPECT_EQ(tokens[12].type, TokenType::END_OF_FILE);
}

TEST(TokenTest, TokenMapTest) {
    // Test d'utilisation des tokens dans une map
    std::map<TokenType, std::string> tokenDescriptions;
    
    tokenDescriptions[TokenType::KW_ONCE] = "Début d'un conte";
    tokenDescriptions[TokenType::KW_IF] = "Condition";
    tokenDescriptions[TokenType::KW_WHILE] = "Boucle";
    tokenDescriptions[TokenType::IDENTIFIER] = "Identifiant";
    
    EXPECT_EQ(tokenDescriptions[TokenType::KW_ONCE], "Début d'un conte");
    EXPECT_EQ(tokenDescriptions[TokenType::KW_IF], "Condition");
    EXPECT_EQ(tokenDescriptions[TokenType::KW_WHILE], "Boucle");
    EXPECT_EQ(tokenDescriptions[TokenType::IDENTIFIER], "Identifiant");
    EXPECT_EQ(tokenDescriptions.size(), 4);
}

TEST(TokenTest, TokenTypeRangeTest) {
    // Test pour vérifier que tous les types de tokens sont correctement convertis en chaînes
    for (int i = static_cast<int>(TokenType::KW_ONCE); 
         i <= static_cast<int>(TokenType::RIGHT_BRACKET); 
         i++) {
        TokenType type = static_cast<TokenType>(i);
        std::string typeStr = tokenTypeToString(type);
        
        // Vérifie que la chaîne n'est pas "UNKNOWN"
        EXPECT_NE(typeStr, "UNKNOWN");
        
        // Vérifie que la chaîne n'est pas vide
        EXPECT_FALSE(typeStr.empty());
    }
}

TEST(TokenTest, TokenComparisonTest) {
    // Test de comparaison de tokens
    Token token1 = {TokenType::IDENTIFIER, "var", 1, 1};
    Token token2 = {TokenType::IDENTIFIER, "var", 1, 1};
    Token token3 = {TokenType::IDENTIFIER, "var", 2, 1};
    Token token4 = {TokenType::NUMBER, "42", 1, 1};
    
    // Comparaison par type
    EXPECT_TRUE(token1.type == token2.type);
    EXPECT_TRUE(token1.type == token3.type);
    EXPECT_FALSE(token1.type == token4.type);
    
    // Comparaison par lexème
    EXPECT_TRUE(token1.lexeme == token2.lexeme);
    EXPECT_TRUE(token1.lexeme == token3.lexeme);
    EXPECT_FALSE(token1.lexeme == token4.lexeme);
    
    // Comparaison par position
    EXPECT_TRUE(token1.line == token2.line && token1.column == token2.column);
    EXPECT_FALSE(token1.line == token3.line);
    EXPECT_TRUE(token1.line == token4.line && token1.column == token4.column);
}

TEST(TokenTest, TokenBoundaryTest) {
    // Test des valeurs limites pour les tokens
    Token emptyToken = {TokenType::IDENTIFIER, "", 0, 0};
    Token longLexeme = {TokenType::STRING, std::string(1000, 'a'), 9999, 9999};
    
    EXPECT_EQ(emptyToken.type, TokenType::IDENTIFIER);
    EXPECT_TRUE(emptyToken.lexeme.empty());
    EXPECT_EQ(emptyToken.line, 0);
    EXPECT_EQ(emptyToken.column, 0);
    
    EXPECT_EQ(longLexeme.type, TokenType::STRING);
    EXPECT_EQ(longLexeme.lexeme.size(), 1000);
    EXPECT_EQ(longLexeme.line, 9999);
    EXPECT_EQ(longLexeme.column, 9999);
} 