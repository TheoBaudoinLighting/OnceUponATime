// code_generator.hpp
#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ast.hpp"
#include <sstream>
#include <string>
#include <set>

class CodeGeneratorVisitor : public AST::Visitor {
public:
    CodeGeneratorVisitor();
    std::string getGeneratedCode() const;
    void visit(AST::NarrativeStatement& node) override;
    void visit(AST::ConditionalStatement& node) override;
    void visit(AST::InteractiveStatement& node) override;
    void visit(AST::RandomStatement& node) override;
    void visit(AST::WhileStatement& node) override;
    void visit(AST::ForEachStatement& node) override;
    void visit(AST::FunctionDeclaration& node) override;
    void visit(AST::FunctionCall& node) override;
    void visit(AST::ReturnStatement& node) override;
    void visit(AST::CommentStatement& node) override;
    void visit(AST::Story& node) override;
    void visit(AST::VariableDeclaration& node) override;
    void visit(AST::VariableDeclarationBlock& node) override;
    void visit(AST::TellStatement& node) override;
private:
    std::ostringstream oss;
    int indentLevel;
    std::set<std::string> collectionsUsed;
    std::string indent() const;
    void generateRandomizer();
    std::string sanitizeIdentifier(const std::string &s) const;
    void collectCollections(AST::Node* node); 
};

#endif
