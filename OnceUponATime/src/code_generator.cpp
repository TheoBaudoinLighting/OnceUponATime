// code_generator.cpp
#include "code_generator.hpp"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <algorithm>

CodeGeneratorVisitor::CodeGeneratorVisitor() : indentLevel(0) {}

std::string CodeGeneratorVisitor::indent() const {
    return std::string(indentLevel * 4, ' ');
}

void CodeGeneratorVisitor::generateRandomizer() {
    oss << "bool getRandomBool() {\n";
    oss << "    return std::rand() % 2 == 0;\n";
    oss << "}\n\n";
}

std::string CodeGeneratorVisitor::getGeneratedCode() const {
    return oss.str();
}

std::string CodeGeneratorVisitor::sanitizeIdentifier(const std::string &s) const {
    std::string result;
    for (char c : s) {
        if (std::isalnum(c))
            result.push_back(std::tolower(c));
        else if (std::isspace(c))
            result.push_back('_');
    }
    return result;
}

void CodeGeneratorVisitor::visit(AST::NarrativeStatement& node) {
    oss << indent() << "std::cout << \"" << node.text << "\" << std::endl;\n";
}

void CodeGeneratorVisitor::visit(AST::ConditionalStatement& node) {
    oss << indent() << "if (" << node.condition << ") {\n";
    indentLevel++;
    for (auto& stmt : node.thenBranch) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}";
    if (!node.elseBranch.empty()) {
        oss << " else {\n";
        indentLevel++;
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
        }
        indentLevel--;
        oss << indent() << "}\n";
    } else {
        oss << "\n";
    }
}

void CodeGeneratorVisitor::visit(AST::InteractiveStatement& node) {
    oss << indent() << "std::cout << \"" << node.prompt << " \";\n";
    oss << indent() << "std::string userInput;\n";
    oss << indent() << "std::getline(std::cin, userInput);\n";
}

void CodeGeneratorVisitor::visit(AST::RandomStatement& node) {
    oss << indent() << "bool randomChoice = getRandomBool();\n";
    oss << indent() << sanitizeIdentifier(node.subject) << "_state = randomChoice ? \"" 
        << node.randomStates.first << "\" : \"" << node.randomStates.second << "\";\n";
    oss << indent() << "std::cout << \"The " << node.subject << " was \" << " 
        << sanitizeIdentifier(node.subject) << "_state << \".\" << std::endl;\n";
}

void CodeGeneratorVisitor::visit(AST::WhileStatement& node) {
    oss << indent() << "while (" << node.condition << ") {\n";
    indentLevel++;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::ForEachStatement& node) {
    std::string collectionName = sanitizeIdentifier(node.collection);
    collectionsUsed.insert(collectionName);
    oss << indent() << "for (auto " << node.iterator << " : " << collectionName << ") {\n";
    indentLevel++;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::FunctionDeclaration& node) {
    oss << indent() << "void " << node.name << "() {\n";
    indentLevel++;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::FunctionCall& node) {
    oss << indent() << node.name << "();\n";
}

void CodeGeneratorVisitor::visit(AST::ReturnStatement& node) {
    oss << indent() << "return;\n";
}

void CodeGeneratorVisitor::visit(AST::CommentStatement& node) {
}

void CodeGeneratorVisitor::visit(AST::Story& node) {
    oss << "#include <iostream>\n";
    oss << "#include <string>\n";
    oss << "#include <vector>\n";
    oss << "#include <cstdlib>\n";
    oss << "#include <ctime>\n\n";
    generateRandomizer();
    oss << "int main() {\n";
    indentLevel++;
    oss << indent() << "std::srand(static_cast<unsigned int>(std::time(nullptr)));\n\n";
    
    collectionsUsed.clear();
    collectCollections(&node);
    
    for (const auto &col : collectionsUsed) {
         oss << indent() << "std::vector<std::string> " << col << " = {};\n";
    }
    if (!collectionsUsed.empty()) {
         oss << "\n";
    }
    
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    oss << "\n" << indent() << "return 0;\n";
    indentLevel--;
    oss << "}\n";
}

void CodeGeneratorVisitor::visit(AST::VariableDeclaration& node) {
    std::string id = sanitizeIdentifier(node.owner) + "_" + sanitizeIdentifier(node.varName);
    bool isNumber = !node.value.empty() && std::all_of(node.value.begin(), node.value.end(), ::isdigit);
    if (isNumber)
        oss << indent() << "int " << id << " = " << node.value << ";\n";
    else
        oss << indent() << "std::string " << id << " = \"" << node.value << "\";\n";
}

void CodeGeneratorVisitor::visit(AST::VariableDeclarationBlock& node) {
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
}

void CodeGeneratorVisitor::collectCollections(AST::Node* node) {
    if (auto fe = dynamic_cast<AST::ForEachStatement*>(node)) {
        collectionsUsed.insert(sanitizeIdentifier(fe->collection));
    }
    if (auto story = dynamic_cast<AST::Story*>(node)) {
        for (auto& stmt : story->statements) {
            collectCollections(stmt.get());
        }
    } else if (auto cond = dynamic_cast<AST::ConditionalStatement*>(node)) {
        for (auto& stmt : cond->thenBranch) {
            collectCollections(stmt.get());
        }
        for (auto& stmt : cond->elseBranch) {
            collectCollections(stmt.get());
        }
    } else if (auto whileStmt = dynamic_cast<AST::WhileStatement*>(node)) {
        for (auto& stmt : whileStmt->body) {
            collectCollections(stmt.get());
        }
    } else if (auto forEach = dynamic_cast<AST::ForEachStatement*>(node)) {
        for (auto& stmt : forEach->body) {
            collectCollections(stmt.get());
        }
    } else if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        for (auto& stmt : funcDecl->body) {
            collectCollections(stmt.get());
        }
    }
}

void CodeGeneratorVisitor::visit(AST::TellStatement& node) {
    oss << indent() << "std::cout << \"" << node.message << "\" << std::endl;\n";
}