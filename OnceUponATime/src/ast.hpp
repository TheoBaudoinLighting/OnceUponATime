// ast.hpp
#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <utility>

namespace AST {

class Visitor;

class Node {
public:
    virtual ~Node() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Statement : public Node {
public:
    virtual ~Statement() = default;
};

class NarrativeStatement : public Statement {
public:
    std::string text;
    NarrativeStatement(std::string text) : text(std::move(text)) {}
    void accept(Visitor& visitor) override;
};

class ConditionalStatement : public Statement {
public:
    std::string condition;
    std::vector<std::unique_ptr<Statement>> thenBranch;
    std::vector<std::unique_ptr<Statement>> elseBranch;
    explicit ConditionalStatement(const std::string &condition) : condition(condition) {}
    void accept(Visitor& visitor) override;
};

class InteractiveStatement : public Statement {
public:
    std::string prompt;
    explicit InteractiveStatement(const std::string &prompt) : prompt(prompt) {}
    void accept(Visitor& visitor) override;
};

class RandomStatement : public Statement {
public:
    std::string subject;
    std::pair<std::string, std::string> randomStates;
    RandomStatement(const std::string &subject, const std::pair<std::string, std::string>& randomStates)
        : subject(subject), randomStates(randomStates) {}
    void accept(Visitor& visitor) override;
};

class WhileStatement : public Statement {
public:
    std::string condition;
    std::vector<std::unique_ptr<Statement>> body;
    explicit WhileStatement(std::string condition) : condition(std::move(condition)) {}
    void accept(Visitor& visitor) override;
};

class ForEachStatement : public Statement {
public:
    std::string iterator;
    std::string collection;
    std::vector<std::unique_ptr<Statement>> body;
    ForEachStatement(std::string iterator, std::string collection)
        : iterator(std::move(iterator)), collection(std::move(collection)) {}
    void accept(Visitor& visitor) override;
};

class FunctionDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::unique_ptr<Statement>> body;
    explicit FunctionDeclaration(std::string name) : name(std::move(name)) {}
    void accept(Visitor& visitor) override;
};

class FunctionCall : public Statement {
public:
    std::string name;
    explicit FunctionCall(std::string name) : name(std::move(name)) {}
    void accept(Visitor& visitor) override;
};

class ReturnStatement : public Statement {
public:
    void accept(Visitor& visitor) override;
};

class CommentStatement : public Statement {
public:
    std::string comment;
    explicit CommentStatement(std::string comment) : comment(std::move(comment)) {}
    void accept(Visitor& visitor) override;
};

class VariableDeclaration : public Statement {
public:
    std::string owner;
    std::string varName;
    std::string value;
    VariableDeclaration(std::string owner, std::string varName, std::string value)
      : owner(std::move(owner)), varName(std::move(varName)), value(std::move(value)) {}
    void accept(Visitor& visitor) override;
};

class VariableDeclarationBlock : public Statement {
public:
    std::vector<std::unique_ptr<VariableDeclaration>> declarations;
    void accept(Visitor& visitor) override;
};

class Story : public Node {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    void accept(Visitor& visitor) override;
};

class TellStatement : public Statement {
public:
    std::string message;
    explicit TellStatement(std::string message) : message(std::move(message)) {}
    void accept(Visitor& visitor) override;
};

class Visitor {
public:
    virtual ~Visitor() = default;
    virtual void visit(NarrativeStatement& node) = 0;
    virtual void visit(ConditionalStatement& node) = 0;
    virtual void visit(InteractiveStatement& node) = 0;
    virtual void visit(RandomStatement& node) = 0;
    virtual void visit(WhileStatement& node) = 0;
    virtual void visit(ForEachStatement& node) = 0;
    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(FunctionCall& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(CommentStatement& node) = 0;
    virtual void visit(Story& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(VariableDeclarationBlock& node) = 0;
    virtual void visit(TellStatement& node) = 0;
};

}

#endif
