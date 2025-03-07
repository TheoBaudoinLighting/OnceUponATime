// ast_tests.cpp

#include <gtest/gtest.h>
#include "../src/ast.hpp"
#include <sstream>
#include <memory>
#include <vector>

class TestVisitor : public AST::Visitor {
public:
    std::ostringstream output;

    void visit(AST::NarrativeStatement& node) override {
        output << "Narrative: " << node.text << "\n";
    }
    void visit(AST::ConditionalStatement& node) override {
        output << "Conditional: " << node.condition << "\n";
        for (auto &stmt : node.thenBranch)
            stmt->accept(*this);
        for (auto &stmt : node.elseBranch)
            stmt->accept(*this);
    }
    void visit(AST::InteractiveStatement& node) override {
        output << "Interactive: " << node.prompt << "\n";
    }
    void visit(AST::RandomStatement& node) override {
        output << "Random: " << node.subject << " " 
               << node.randomStates.first << " " 
               << node.randomStates.second << "\n";
    }
    void visit(AST::WhileStatement& node) override {
        output << "While: " << node.condition << "\n";
        for (auto &stmt : node.body)
            stmt->accept(*this);
    }
    void visit(AST::ForEachStatement& node) override {
        output << "ForEach: " << node.iterator << " in " << node.collection << "\n";
        for (auto &stmt : node.body)
            stmt->accept(*this);
    }
    void visit(AST::FunctionDeclaration& node) override {
        output << "FunctionDeclaration: " << node.name << "\n";
        for (auto &stmt : node.body)
            stmt->accept(*this);
    }
    void visit(AST::FunctionCall& node) override {
        output << "FunctionCall: " << node.name << "\n";
    }
    void visit(AST::ReturnStatement& node) override {
        output << "Return\n";
    }
    void visit(AST::CommentStatement& node) override {
        output << "Comment: " << node.comment << "\n";
    }
    void visit(AST::Story& node) override {
        output << "Story:\n";
        for (auto &stmt : node.statements)
            stmt->accept(*this);
    }
    void visit(AST::VariableDeclaration& node) override {
        output << "VariableDeclaration: " << node.owner << " " << node.varName << " " << node.value << "\n";
    }
    void visit(AST::VariableDeclarationBlock& node) override {
        output << "VariableDeclarationBlock:\n";
        for (auto &decl : node.declarations)
            decl->accept(*this);
    }
    void visit(AST::TellStatement& node) override {
        output << "Tell: " << node.message << "\n";
    }
};

TEST(ASTTest, NarrativeStatementTest) {
    AST::NarrativeStatement n("princess lived happily");
    TestVisitor visitor;
    n.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("Narrative: princess lived happily"), std::string::npos);
}

TEST(ASTTest, ConditionalStatementTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("door opens");
    AST::ConditionalStatement cond("door is unlocked");
    cond.thenBranch.push_back(std::move(narrative));
    TestVisitor visitor;
    cond.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("Conditional: door is unlocked"), std::string::npos);
    EXPECT_NE(result.find("Narrative: door opens"), std::string::npos);
}

TEST(ASTTest, WhileStatementTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("dragon roars loudly");
    AST::WhileStatement ws("dragon is awake");
    ws.body.push_back(std::move(narrative));
    TestVisitor visitor;
    ws.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("While: dragon is awake"), std::string::npos);
    EXPECT_NE(result.find("Narrative: dragon roars loudly"), std::string::npos);
}

TEST(ASTTest, ForEachStatementTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("companion joins bravely");
    AST::ForEachStatement fe("companion", "squad");
    fe.body.push_back(std::move(narrative));
    TestVisitor visitor;
    fe.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("ForEach: companion in squad"), std::string::npos);
    EXPECT_NE(result.find("Narrative: companion joins bravely"), std::string::npos);
}

TEST(ASTTest, FunctionDeclarationAndCallTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("hero heals quickly");
    auto funcDecl = std::make_unique<AST::FunctionDeclaration>("healHero");
    funcDecl->body.push_back(std::move(narrative));
    AST::FunctionCall funcCall("healHero");
    TestVisitor visitor;
    funcDecl->accept(visitor);
    funcCall.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("FunctionDeclaration: healHero"), std::string::npos);
    EXPECT_NE(result.find("FunctionCall: healHero"), std::string::npos);
}

TEST(ASTTest, ReturnAndCommentTest) {
    AST::ReturnStatement ret;
    AST::CommentStatement comment("This is a test comment.");
    TestVisitor visitor;
    ret.accept(visitor);
    comment.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("Return"), std::string::npos);
    EXPECT_NE(result.find("Comment: This is a test comment."), std::string::npos);
}

TEST(ASTTest, VariableDeclarationTest) {
    AST::VariableDeclaration varDecl("The hero", "strength", "10");
    TestVisitor visitor;
    varDecl.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("VariableDeclaration: The hero strength 10"), std::string::npos);
}

TEST(ASTTest, VariableDeclarationBlockTest) {
    auto varDecl1 = std::make_unique<AST::VariableDeclaration>("The hero", "strength", "10");
    auto varDecl2 = std::make_unique<AST::VariableDeclaration>("The hero", "magic", "5");
    auto varBlock = std::make_unique<AST::VariableDeclarationBlock>();
    varBlock->declarations.push_back(std::move(varDecl1));
    varBlock->declarations.push_back(std::move(varDecl2));
    TestVisitor visitor;
    varBlock->accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("VariableDeclarationBlock:"), std::string::npos);
    EXPECT_NE(result.find("The hero strength 10"), std::string::npos);
    EXPECT_NE(result.find("The hero magic 5"), std::string::npos);
}

TEST(ASTTest, TellStatementTest) {
    AST::TellStatement tell("Hello, world!");
    TestVisitor visitor;
    tell.accept(visitor);
    std::string result = visitor.output.str();
    EXPECT_NE(result.find("Tell: Hello, world!"), std::string::npos);
}
