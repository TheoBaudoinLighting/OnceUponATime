// compiler_tests.cpp

#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/code_generator.hpp"
#include "../src/ast.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <iostream>

std::unique_ptr<AST::Story> compileScript(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parseStory();
}

TEST(CompilerTest, FullCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "The hero lived bravely. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int main()"), std::string::npos);
    EXPECT_NE(generated.find("The hero lived bravely"), std::string::npos);
}

TEST(CompilerTest, ConditionalCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "If hero is brave then "
        "The hero wins. "
        "End. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("if (hero is brave)"), std::string::npos);
    EXPECT_NE(generated.find("The hero wins"), std::string::npos);
}

TEST(CompilerTest, LoopCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "While dragon is awake. "
        "Hero trembles. "
        "Endwhile. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while (dragon is awake)"), std::string::npos);
    EXPECT_NE(generated.find("Hero trembles"), std::string::npos);
}

TEST(CompilerTest, FunctionCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "Define the function healHero as "
        "Hero recovers health. "
        "Endfunction. "
        "Call healHero. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("void healHero()"), std::string::npos);
    EXPECT_NE(generated.find("Hero recovers health"), std::string::npos);
    EXPECT_NE(generated.find("healHero();"), std::string::npos);
}

TEST(CompilerTest, VariableCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "The hero has strength of 10. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int the_hero_strength = 10;"), std::string::npos);
}

TEST(CompilerTest, NestedWhileCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "While dragon is awake. "
        "While hero is brave. "
        "Hero fights. "
        "Endwhile. "
        "Endwhile. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while (dragon is awake)"), std::string::npos);
    EXPECT_NE(generated.find("while (hero is brave)"), std::string::npos);
    EXPECT_NE(generated.find("Hero fights"), std::string::npos);
}

TEST(CompilerTest, NestedForEachCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "For each castle in kingdom do "
        "For each room in castle do "
        "Room is cleaned. "
        "Endfor. "
        "Endfor. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (auto castle : kingdom)"), std::string::npos);
    EXPECT_NE(generated.find("for (auto room : castle)"), std::string::npos);
    EXPECT_NE(generated.find("Room is cleaned"), std::string::npos);
}

TEST(CompilerTest, LoopWithConditionalCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "For each knight in round table do "
        "If knight is brave then "
        "Knight fights. "
        "End. "
        "Endfor. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (auto knight : round_table)"), std::string::npos);
    EXPECT_NE(generated.find("if (knight is brave)"), std::string::npos);
    EXPECT_NE(generated.find("Knight fights"), std::string::npos);
}

TEST(CompilerTest, WhileMultipleStatementsCompilationTest) {
    std::string script = 
        "Once upon a time. "
        "While dragon is awake. "
        "Hero trembles. "
        "Knight prepares. "
        "Wizard casts spell. "
        "Endwhile. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while (dragon is awake)"), std::string::npos);
    EXPECT_NE(generated.find("Hero trembles"), std::string::npos);
    EXPECT_NE(generated.find("Knight prepares"), std::string::npos);
    EXPECT_NE(generated.find("Wizard casts spell"), std::string::npos);
}

TEST(CompilerTest, ForEachWithUndeclaredCollectionTest) {
    std::string script = 
        "Once upon a time. "
        "For each companion in squad do "
        "Tell \"Hello\". "
        "Endfor. "
        "The story ends.";
    auto story = compileScript(script);
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    
    EXPECT_NE(generated.find("std::vector<std::string> squad = {}"), std::string::npos);
}
