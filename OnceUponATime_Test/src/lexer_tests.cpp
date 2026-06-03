// lexer_tests.cpp

#include "pch.h"

#include "lexer.h"
#include "token.h"
#include <string>
#include <vector>

TEST(LexerTest, PrologueTokenizationTest) {
    std::string source = "Once upon a time.";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, TokenType::KW_ONCE);
    EXPECT_EQ(tokens[1].type, TokenType::KW_UPON);
    EXPECT_EQ(tokens[2].type, TokenType::KW_A);
    EXPECT_EQ(tokens[3].type, TokenType::KW_TIME);
    EXPECT_EQ(tokens[4].type, TokenType::PERIOD);
    EXPECT_EQ(tokens[5].type, TokenType::END_OF_FILE);
}

TEST(LexerTest, NumberTokenizationTest) {
    std::string source = "12345";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "12345");
}

TEST(LexerTest, DecimalNumberTokenizationTest) {
    std::string source = "3.14";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::NUMBER);
    EXPECT_EQ(tokens[0].lexeme, "3.14");
}

TEST(LexerTest, StringTokenizationTest) {
    std::string source = "\"Hello, world!\"";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::STRING);
    EXPECT_EQ(tokens[0].lexeme, "Hello, world!");
}

TEST(LexerTest, IdentifierTokenizationTest) {
    std::string source = "princess";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[0].lexeme, "princess");
}

TEST(LexerTest, CommaIsSeparatePunctuationTest) {
    std::string source = "Alice, Bob";
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    ASSERT_GE(tokens.size(), 4);
    EXPECT_EQ(tokens[0].type, TokenType::IDENTIFIER);
    EXPECT_EQ(tokens[1].type, TokenType::COMMA);
    EXPECT_EQ(tokens[2].type, TokenType::IDENTIFIER);
}
