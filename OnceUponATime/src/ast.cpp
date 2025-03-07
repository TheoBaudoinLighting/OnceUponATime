// ast.cpp
#include "ast.hpp"

namespace AST {

void NarrativeStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ConditionalStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void InteractiveStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void RandomStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void WhileStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ForEachStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void FunctionDeclaration::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void FunctionCall::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void ReturnStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void CommentStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void VariableDeclaration::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void VariableDeclarationBlock::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void Story::accept(Visitor& visitor) {
    visitor.visit(*this);
}

void TellStatement::accept(Visitor& visitor) {
    visitor.visit(*this);
}

}
