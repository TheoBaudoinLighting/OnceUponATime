// code_generator_tests.cpp

#include <gtest/gtest.h>
#include "../src/code_generator.hpp"
#include "../src/ast.hpp"
#include <memory>
#include <string>
#include <vector>

TEST(CodeGeneratorTest, NarrativeGenerationTest) {
    AST::NarrativeStatement narrative("princess lived happily");
    CodeGeneratorVisitor codeGen;
    narrative.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("princess lived happily"), std::string::npos);
}

TEST(CodeGeneratorTest, ConditionalGenerationTest) {
    auto narrativeThen = std::make_unique<AST::NarrativeStatement>("door opens");
    AST::ConditionalStatement cond("door is unlocked");
    cond.thenBranch.push_back(std::move(narrativeThen));
    CodeGeneratorVisitor codeGen;
    cond.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("if ("), std::string::npos);
    EXPECT_NE(generated.find("door is unlocked"), std::string::npos);
}

TEST(CodeGeneratorTest, WhileAndForEachGenerationTest) {
    auto narrativeWhile = std::make_unique<AST::NarrativeStatement>("dragon roars loudly");
    auto whileStmt = std::make_unique<AST::WhileStatement>("dragon is awake");
    whileStmt->body.push_back(std::move(narrativeWhile));

    auto narrativeForEach = std::make_unique<AST::NarrativeStatement>("companion joins bravely");
    auto foreachStmt = std::make_unique<AST::ForEachStatement>("companion", "squad");
    foreachStmt->body.push_back(std::move(narrativeForEach));

    CodeGeneratorVisitor codeGen;
    whileStmt->accept(codeGen);
    foreachStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while ("), std::string::npos);
    EXPECT_NE(generated.find("for (auto companion : squad)"), std::string::npos);
}

TEST(CodeGeneratorTest, FunctionGenerationTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("hero heals quickly");
    auto funcDecl = std::make_unique<AST::FunctionDeclaration>("healHero");
    funcDecl->body.push_back(std::move(narrative));

    CodeGeneratorVisitor codeGen;
    funcDecl->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("void healHero()"), std::string::npos);
}

TEST(CodeGeneratorTest, VariableDeclarationGenerationTest) {
    AST::VariableDeclaration varDecl("The hero", "strength", "10");
    CodeGeneratorVisitor codeGen;
    varDecl.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int the_hero_strength = 10;"), std::string::npos);
}

TEST(CodeGeneratorTest, StoryGenerationTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("princess lived bravely");
    AST::Story story;
    story.statements.push_back(std::move(narrative));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int main()"), std::string::npos);
    EXPECT_NE(generated.find("princess lived bravely"), std::string::npos);
}

TEST(CodeGeneratorTest, InteractiveGenerationTest) {
    AST::InteractiveStatement interactive("What is your name?");
    CodeGeneratorVisitor codeGen;
    interactive.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("std::cout <<"), std::string::npos);
    EXPECT_NE(generated.find("What is your name?"), std::string::npos);
    EXPECT_NE(generated.find("std::getline(std::cin, userInput)"), std::string::npos);
}

TEST(CodeGeneratorTest, RandomGenerationTest) {
    AST::RandomStatement random("dragon", std::make_pair("friendly", "hostile"));
    CodeGeneratorVisitor codeGen;
    random.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("getRandomBool()"), std::string::npos);
    EXPECT_NE(generated.find("dragon_state"), std::string::npos);
    EXPECT_NE(generated.find("friendly"), std::string::npos);
    EXPECT_NE(generated.find("hostile"), std::string::npos);
}

TEST(CodeGeneratorTest, FunctionCallGenerationTest) {
    AST::FunctionCall funcCall("healHero");
    CodeGeneratorVisitor codeGen;
    funcCall.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("healHero();"), std::string::npos);
}

TEST(CodeGeneratorTest, ReturnStatementGenerationTest) {
    AST::ReturnStatement returnStmt;
    CodeGeneratorVisitor codeGen;
    returnStmt.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("return;"), std::string::npos);
}

TEST(CodeGeneratorTest, CommentGenerationTest) {
    AST::CommentStatement comment("This is a test comment");
    CodeGeneratorVisitor codeGen;
    comment.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_EQ(generated.find("This is a test comment"), std::string::npos);
}

TEST(CodeGeneratorTest, VariableDeclarationBlockGenerationTest) {
    auto varDecl1 = std::make_unique<AST::VariableDeclaration>("The hero", "strength", "10");
    auto varDecl2 = std::make_unique<AST::VariableDeclaration>("The hero", "magic", "5");
    auto varBlock = std::make_unique<AST::VariableDeclarationBlock>();
    varBlock->declarations.push_back(std::move(varDecl1));
    varBlock->declarations.push_back(std::move(varDecl2));
    
    CodeGeneratorVisitor codeGen;
    varBlock->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int the_hero_strength = 10;"), std::string::npos);
    EXPECT_NE(generated.find("int the_hero_magic = 5;"), std::string::npos);
}

TEST(CodeGeneratorTest, WhileMultipleStatementsGenerationTest) {
    auto whileStmt = std::make_unique<AST::WhileStatement>("dragon is awake");
    whileStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Hero trembles"));
    whileStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Knight prepares"));
    
    CodeGeneratorVisitor codeGen;
    whileStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while (dragon is awake)"), std::string::npos);
    EXPECT_NE(generated.find("Hero trembles"), std::string::npos);
    EXPECT_NE(generated.find("Knight prepares"), std::string::npos);
}

TEST(CodeGeneratorTest, NestedWhileGenerationTest) {
    auto innerWhile = std::make_unique<AST::WhileStatement>("hero is brave");
    innerWhile->body.push_back(std::make_unique<AST::NarrativeStatement>("Hero fights"));
    
    auto outerWhile = std::make_unique<AST::WhileStatement>("dragon is awake");
    outerWhile->body.push_back(std::move(innerWhile));
    
    CodeGeneratorVisitor codeGen;
    outerWhile->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while (dragon is awake)"), std::string::npos);
    EXPECT_NE(generated.find("while (hero is brave)"), std::string::npos);
    EXPECT_NE(generated.find("Hero fights"), std::string::npos);
}

TEST(CodeGeneratorTest, ForEachMultipleStatementsGenerationTest) {
    auto forEachStmt = std::make_unique<AST::ForEachStatement>("knight", "round table");
    forEachStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Knight stands"));
    forEachStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Knight bows"));
    
    CodeGeneratorVisitor codeGen;
    forEachStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (auto knight : round_table)"), std::string::npos);
    EXPECT_NE(generated.find("Knight stands"), std::string::npos);
    EXPECT_NE(generated.find("Knight bows"), std::string::npos);
}

TEST(CodeGeneratorTest, NestedForEachGenerationTest) {
    auto innerForEach = std::make_unique<AST::ForEachStatement>("room", "castle");
    innerForEach->body.push_back(std::make_unique<AST::NarrativeStatement>("Room is cleaned"));
    
    auto outerForEach = std::make_unique<AST::ForEachStatement>("castle", "kingdom");
    outerForEach->body.push_back(std::move(innerForEach));
    
    CodeGeneratorVisitor codeGen;
    outerForEach->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (auto castle : kingdom)"), std::string::npos);
    EXPECT_NE(generated.find("for (auto room : castle)"), std::string::npos);
    EXPECT_NE(generated.find("Room is cleaned"), std::string::npos);
}

TEST(CodeGeneratorTest, LoopWithConditionalGenerationTest) {
    auto conditional = std::make_unique<AST::ConditionalStatement>("knight is brave");
    conditional->thenBranch.push_back(std::make_unique<AST::NarrativeStatement>("Knight fights"));
    
    auto forEachStmt = std::make_unique<AST::ForEachStatement>("knight", "round table");
    forEachStmt->body.push_back(std::move(conditional));
    
    CodeGeneratorVisitor codeGen;
    forEachStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (auto knight : round_table)"), std::string::npos);
    EXPECT_NE(generated.find("if (knight is brave)"), std::string::npos);
    EXPECT_NE(generated.find("Knight fights"), std::string::npos);
}
