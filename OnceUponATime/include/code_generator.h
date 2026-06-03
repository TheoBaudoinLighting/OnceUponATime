// code_generator.hpp
#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include "ast.h"
#include <map>
#include <sstream>
#include <set>
#include <string>
#include <vector>

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
    void visit(AST::ForRangeStatement& node) override;
    void visit(AST::FunctionDeclaration& node) override;
    void visit(AST::FunctionCall& node) override;
    void visit(AST::ReturnStatement& node) override;
    void visit(AST::CommentStatement& node) override;
    void visit(AST::Story& node) override;
    void visit(AST::VariableDeclaration& node) override;
    void visit(AST::VariableDeclarationBlock& node) override;
    void visit(AST::ArithmeticStatement& node) override;
    void visit(AST::RecordDeclaration& node) override;
    void visit(AST::RecordInstanceDeclaration& node) override;
    void visit(AST::ImageDeclaration& node) override;
    void visit(AST::PixelWriteStatement& node) override;
    void visit(AST::ImageFillStatement& node) override;
    void visit(AST::RectanglePaintStatement& node) override;
    void visit(AST::ImageSaveStatement& node) override;
    void visit(AST::TellStatement& node) override;
private:
    struct RecordField {
        std::string sourceName;
        std::string cppName;
        std::string typeName;
    };

    std::ostringstream oss;
    int indentLevel;
    std::set<std::string> collectionsUsed;
    std::set<std::string> declaredCollections;
    std::map<std::string, std::string> symbols;
    std::map<std::string, std::string> symbolKinds;
    std::map<std::string, std::vector<RecordField>> recordTypes;
    std::map<std::string, std::string> recordTypeAliases;
    std::set<std::string> initializedSymbols;
    bool skipFunctionDeclarations;
    bool skipRecordDeclarations;
    bool imageRuntimeRequired;
    int tempCounter;
    std::string indent() const;
    void generateRandomizer();
    void generateStoryStateHelpers();
    void generateImageRuntime();
    std::string sanitizeIdentifier(const std::string &s) const;
    std::string sanitizeTypeName(const std::string& s) const;
    std::string escapeString(const std::string& s) const;
    std::string normalizeName(const std::string& s) const;
    std::string variableNameFor(const AST::VariableDeclaration& node) const;
    std::string variableNameFor(const AST::RecordInstanceDeclaration& node) const;
    std::string resolveName(const std::string& name) const;
    std::string translateCondition(const std::string& condition) const;
    std::string numericExpression(const std::string& value) const;
    std::string typedExpression(const std::string& value, const std::string& typeName) const;
    std::string cppTypeFor(const std::string& typeName) const;
    std::string cppDefaultValueFor(const std::string& typeName) const;
    std::string kindForType(const std::string& typeName) const;
    std::string fieldTypeFor(const std::string& recordType, const std::string& fieldName) const;
    void registerDeclaration(const AST::VariableDeclaration& node);
    void registerArithmeticTarget(const AST::ArithmeticStatement& node);
    void registerRecordType(const AST::RecordDeclaration& node);
    void registerRecordInstance(const AST::RecordInstanceDeclaration& node);
    void registerRecordFieldSymbols(const std::string& sourcePrefix, const std::string& cppPrefix,
                                    const std::string& recordType, int depth);
    void collectDeclarations(AST::Node* node);
    void collectCollections(AST::Node* node);
    void collectRecords(AST::Node* node, std::vector<AST::RecordDeclaration*>& records);
    void collectFunctions(AST::Node* node, std::vector<AST::FunctionDeclaration*>& functions);
    void collectImageUsage(AST::Node* node);
};

#endif
