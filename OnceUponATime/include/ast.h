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
    std::vector<std::string> values;
    VariableDeclaration(std::string owner, std::string varName, std::string value)
      : owner(std::move(owner)), varName(std::move(varName)), value(std::move(value)) {}
    VariableDeclaration(std::string owner, std::string varName, std::vector<std::string> values)
      : owner(std::move(owner)), varName(std::move(varName)), values(std::move(values)) {}
    bool isCollection() const { return !values.empty(); }
    void accept(Visitor& visitor) override;
};

class ForRangeStatement : public Statement {
public:
    std::string iterator;
    std::string start;
    std::string end;
    std::vector<std::unique_ptr<Statement>> body;
    ForRangeStatement(std::string iterator, std::string start, std::string end)
        : iterator(std::move(iterator)),
          start(std::move(start)),
          end(std::move(end)) {}
    void accept(Visitor& visitor) override;
};

class VariableDeclarationBlock : public Statement {
public:
    std::vector<std::unique_ptr<VariableDeclaration>> declarations;
    void accept(Visitor& visitor) override;
};

class ArithmeticStatement : public Statement {
public:
    std::string left;
    std::string operation;
    std::string right;
    std::string target;
    ArithmeticStatement(std::string left, std::string operation, std::string right, std::string target)
        : left(std::move(left)),
          operation(std::move(operation)),
          right(std::move(right)),
          target(std::move(target)) {}
    void accept(Visitor& visitor) override;
};

class RecordDeclaration : public Statement {
public:
    std::string name;
    std::vector<std::pair<std::string, std::string>> fields;
    RecordDeclaration(std::string name, std::vector<std::pair<std::string, std::string>> fields)
        : name(std::move(name)), fields(std::move(fields)) {}
    void accept(Visitor& visitor) override;
};

class RecordInstanceDeclaration : public Statement {
public:
    std::string name;
    std::string typeName;
    std::vector<std::pair<std::string, std::string>> fieldValues;
    RecordInstanceDeclaration(std::string name, std::string typeName,
                              std::vector<std::pair<std::string, std::string>> fieldValues)
        : name(std::move(name)),
          typeName(std::move(typeName)),
          fieldValues(std::move(fieldValues)) {}
    void accept(Visitor& visitor) override;
};

class ImageDeclaration : public Statement {
public:
    std::string name;
    std::string width;
    std::string height;
    ImageDeclaration(std::string name, std::string width, std::string height)
        : name(std::move(name)), width(std::move(width)), height(std::move(height)) {}
    void accept(Visitor& visitor) override;
};

class PixelWriteStatement : public Statement {
public:
    std::string imageName;
    std::string x;
    std::string y;
    std::string red;
    std::string green;
    std::string blue;
    PixelWriteStatement(std::string imageName, std::string x, std::string y,
                        std::string red, std::string green, std::string blue)
        : imageName(std::move(imageName)),
          x(std::move(x)),
          y(std::move(y)),
                        red(std::move(red)),
                        green(std::move(green)),
                        blue(std::move(blue)) {}
    void accept(Visitor& visitor) override;
};

class ImageFillStatement : public Statement {
public:
    std::string imageName;
    std::string red;
    std::string green;
    std::string blue;
    ImageFillStatement(std::string imageName, std::string red, std::string green, std::string blue)
        : imageName(std::move(imageName)),
          red(std::move(red)),
          green(std::move(green)),
          blue(std::move(blue)) {}
    void accept(Visitor& visitor) override;
};

class RectanglePaintStatement : public Statement {
public:
    std::string imageName;
    std::string left;
    std::string bottom;
    std::string right;
    std::string top;
    std::string red;
    std::string green;
    std::string blue;
    RectanglePaintStatement(std::string imageName, std::string left, std::string bottom,
                            std::string right, std::string top,
                            std::string red, std::string green, std::string blue)
        : imageName(std::move(imageName)),
          left(std::move(left)),
          bottom(std::move(bottom)),
          right(std::move(right)),
          top(std::move(top)),
          red(std::move(red)),
          green(std::move(green)),
          blue(std::move(blue)) {}
    void accept(Visitor& visitor) override;
};

class ImageSaveStatement : public Statement {
public:
    std::string imageName;
    std::string outputPath;
    ImageSaveStatement(std::string imageName, std::string outputPath)
        : imageName(std::move(imageName)), outputPath(std::move(outputPath)) {}
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
    virtual void visit(ForRangeStatement& node) = 0;
    virtual void visit(FunctionDeclaration& node) = 0;
    virtual void visit(FunctionCall& node) = 0;
    virtual void visit(ReturnStatement& node) = 0;
    virtual void visit(CommentStatement& node) = 0;
    virtual void visit(Story& node) = 0;
    virtual void visit(VariableDeclaration& node) = 0;
    virtual void visit(VariableDeclarationBlock& node) = 0;
    virtual void visit(ArithmeticStatement& node) = 0;
    virtual void visit(RecordDeclaration& node) = 0;
    virtual void visit(RecordInstanceDeclaration& node) = 0;
    virtual void visit(ImageDeclaration& node) = 0;
    virtual void visit(PixelWriteStatement& node) = 0;
    virtual void visit(ImageFillStatement& node) = 0;
    virtual void visit(RectanglePaintStatement& node) = 0;
    virtual void visit(ImageSaveStatement& node) = 0;
    virtual void visit(TellStatement& node) = 0;
};

}

#endif
