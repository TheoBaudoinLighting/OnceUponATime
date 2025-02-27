// code_generator.hpp

#ifndef CODE_GENERATOR_HPP
#define CODE_GENERATOR_HPP

#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <unordered_map>
#include "parser.hpp"

class CodeGenerator {
public:
    CodeGenerator(const std::vector<Sentence>& sentences);
    std::string generateCode() const;
    
    void debugPrintSentenceStructure(const Sentence& sentence, int level = 0) const;
    void debugPrintEntityStates() const;

private:
    const std::vector<Sentence>& sentences;
    std::set<std::string> entityNames;
    mutable std::unordered_map<std::string, bool> entityStates;
    
    void analyzeEntities();
    void analyzeCondition(const std::string& condition);
    void generateCodeForSentence(std::ostringstream& oss, const Sentence& sentence, int indentLevel) const;
    std::string translateCondition(const std::string& condition) const;
    std::string normalizeIdentifier(const std::string& text) const;
    std::string stripArticle(const std::string& text) const;
    std::string toLower(const std::string& text) const;
    std::string generateStatement(const Sentence& sentence) const;
    void generateRandomizer(std::ostringstream& oss) const;
    void handleRandomSentence(std::ostringstream& oss, const Sentence& sentence, int indentLevel) const;
};

#endif // CODE_GENERATOR_HPP