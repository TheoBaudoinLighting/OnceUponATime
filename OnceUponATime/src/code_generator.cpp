// code_generator.cpp

#include "code_generator.hpp"
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <cctype>
#include <random>
#include <ctime>

CodeGenerator::CodeGenerator(const std::vector<Sentence>& sentences)
    : sentences(sentences) {
    analyzeEntities();
}

void CodeGenerator::analyzeEntities() {
    for (const auto& sentence : sentences) {
        if (!sentence.subject.empty()) {
            std::string normalizedSubject = normalizeIdentifier(sentence.subject);
            entityNames.insert(normalizedSubject);

            if (!sentence.isConditional && !sentence.verb.empty() && (!sentence.adjectives.empty() || !sentence.object.empty())) {
                std::string verb = toLower(sentence.verb);
                if (verb == "was" || verb == "is" || verb == "became") {
                    bool hasNegation = false;
                    std::string stateStr;
                    
                    if (!sentence.adjectives.empty() && toLower(sentence.adjectives[0]) == "not" && !sentence.object.empty()) {
                        hasNegation = true;
                        stateStr = sentence.object;
                    } else if (!sentence.adjectives.empty()) {
                        for (size_t i = 0; i < sentence.adjectives.size(); i++) {
                            std::string adjective = toLower(sentence.adjectives[i]);
                            if (adjective == "not" && i + 1 < sentence.adjectives.size()) {
                                hasNegation = true;
                                stateStr = sentence.adjectives[i + 1];
                                break;
                            } else if (i == sentence.adjectives.size() - 1) {
                                stateStr = sentence.adjectives[i];
                            }
                        }
                    } else if (!sentence.object.empty()) {
                        stateStr = sentence.object;
                    }
                    
                    std::string normalizedState = normalizeIdentifier(stateStr);
                    std::string stateVar = normalizedSubject + "_is_" + normalizedState;
                    entityStates[stateVar] = !hasNegation;
                }
            }
        }
        
        if (sentence.isRandom && !sentence.subject.empty() && 
            !sentence.randomStates.first.empty() && !sentence.randomStates.second.empty()) {
            std::string normalizedSubject = normalizeIdentifier(stripArticle(sentence.subject));
            std::string normalizedState1 = normalizeIdentifier(sentence.randomStates.first);
            std::string normalizedState2 = normalizeIdentifier(sentence.randomStates.second);
            
            std::string stateVar1 = normalizedSubject + "_is_" + normalizedState1;
            std::string stateVar2 = normalizedSubject + "_is_" + normalizedState2;
            
            entityStates[stateVar1] = false;
            entityStates[stateVar2] = false;
        }
        
        if (sentence.isConditional && !sentence.condition.empty()) {
            analyzeCondition(sentence.condition);
        }
        
        if (!sentence.object.empty()) {
            std::string normalizedObject = normalizeIdentifier(sentence.object);
            entityNames.insert(normalizedObject);
        }
    }
}

void CodeGenerator::analyzeCondition(const std::string& condition) {
    std::istringstream iss(condition);
    std::string word;
    std::vector<std::string> words;
    
    while (iss >> word) {
        words.push_back(word);
    }
    
    if (words.size() >= 3) {
        std::string subject = words[0];
        if (toLower(subject) == "the" && words.size() >= 4) {
            subject = words[1];
            std::string verb = toLower(words[2]);
            
            if (verb == "was" || verb == "is") {
                bool hasNegation = false;
                std::string state;
                
                if (words.size() >= 5 && toLower(words[3]) == "not") {
                    hasNegation = true;
                    state = words[4];
                } else {
                    state = words[3];
                }
                
                std::string normalizedSubject = normalizeIdentifier(subject);
                std::string normalizedState = normalizeIdentifier(state);
                entityNames.insert(normalizedSubject);
                
                std::string stateVar = normalizedSubject + "_is_" + normalizedState;
                entityStates[stateVar] = !hasNegation;
            }
        }
    }
}

std::string CodeGenerator::generateCode() const {
    std::ostringstream oss;
    
    oss << "#include <iostream>\n";
    oss << "#include <string>\n";
    oss << "#include <map>\n";
    oss << "#include <random>\n";
    oss << "#include <ctime>\n\n";

    generateRandomizer(oss);
    
    oss << "int main() {\n";
    oss << "    // Initialize the random number generator\n";
    oss << "    std::srand(static_cast<unsigned int>(std::time(nullptr)));\n\n";
    
    std::unordered_map<std::string, bool> runtimeEntityStates = entityStates;
    
    for (const auto& sentence : sentences) {
        if (!sentence.isConditional && !sentence.subject.empty() && !sentence.verb.empty() && !sentence.adjectives.empty()) {
            std::string verb = toLower(sentence.verb);
            if (verb == "was" || verb == "is" || verb == "became") {
                bool hasNegation = false;
                std::string adjectiveStr;
                
                for (size_t i = 0; i < sentence.adjectives.size(); i++) {
                    std::string adjective = toLower(sentence.adjectives[i]);
                    
                    if (adjective == "not" && i + 1 < sentence.adjectives.size()) {
                        hasNegation = true;
                        adjectiveStr = sentence.adjectives[i + 1];
                        break;
                    } else if (i == sentence.adjectives.size() - 1) {
                        adjectiveStr = sentence.adjectives[i];
                    }
                }
                
                std::string normalizedSubject = normalizeIdentifier(stripArticle(sentence.subject));
                std::string normalizedState = normalizeIdentifier(adjectiveStr);
                std::string stateVar = normalizedSubject + "_is_" + normalizedState;
                
                if (runtimeEntityStates.find(stateVar) != runtimeEntityStates.end()) {
                    runtimeEntityStates[stateVar] = !hasNegation;
                }
            }
        }
    }
    
    oss << "    // Entity state declarations\n";
    for (const auto& state : runtimeEntityStates) {
        oss << "    bool " << state.first << " = " << (state.second ? "true" : "false") << ";\n";
    }
    oss << "\n";
    
    for (const auto& sentence : sentences) {
        generateCodeForSentence(oss, sentence, 1);
    }
    
    oss << "    return 0;\n";
    oss << "}\n";
    
    return oss.str();
}

void CodeGenerator::generateRandomizer(std::ostringstream& oss) const {
    oss << "// Function to generate a random boolean\n";
    oss << "bool getRandomBool() {\n";
    oss << "    return std::rand() % 2 == 0;\n";
    oss << "}\n\n";
}

void CodeGenerator::generateCodeForSentence(std::ostringstream& oss, const Sentence& sentence, int indentLevel) const {
    std::string indent(indentLevel * 4, ' ');

    if (sentence.isRandom) {
        handleRandomSentence(oss, sentence, indentLevel);
        return;
    }

    if (sentence.isConditional) {
        std::string translatedCondition = translateCondition(sentence.condition);
        oss << indent << "if (" << translatedCondition << ") {\n";
        for (const auto& thenSentence : sentence.thenBranch) {
            generateCodeForSentence(oss, thenSentence, indentLevel + 1);
        }
        if (!sentence.elseBranch.empty()) {
            if (sentence.elseBranch.size() == 1 && sentence.elseBranch[0].isConditional) {
                const Sentence& elseIfSentence = sentence.elseBranch[0];
                std::string elseIfCondition = translateCondition(elseIfSentence.condition);
                oss << indent << "} else if (" << elseIfCondition << ") {\n";
                for (const auto& thenSentence : elseIfSentence.thenBranch) {
                    generateCodeForSentence(oss, thenSentence, indentLevel + 1);
                }
                if (!elseIfSentence.elseBranch.empty()) {
                    oss << indent << "} else {\n";
                    for (const auto& elseSentence : elseIfSentence.elseBranch) {
                        generateCodeForSentence(oss, elseSentence, indentLevel + 1);
                    }
                }
            } else {
                oss << indent << "} else {\n";
                for (const auto& elseSentence : sentence.elseBranch) {
                    generateCodeForSentence(oss, elseSentence, indentLevel + 1);
                }
            }
        }
        oss << indent << "}\n";
        return;
    }

    if (sentence.isInteractive) {
        oss << indent << "std::cout << \"" << sentence.object << " \";\n";
        oss << indent << "std::string userInput;\n";
        oss << indent << "std::getline(std::cin, userInput);\n";
        return;
    }

    if (sentence.subject.empty()) {
        return;
    }

    oss << indent << "std::cout << \"";
    if (!sentence.verb.empty()) {
        oss << sentence.subject << " " << sentence.verb;
        if (!sentence.adjectives.empty()) {
            oss << " ";
            for (size_t i = 0; i < sentence.adjectives.size(); i++) {
                oss << sentence.adjectives[i];
                if (i < sentence.adjectives.size() - 1)
                    oss << " ";
            }
        }
        if (!sentence.object.empty()) {
            oss << " " << sentence.object;
        }
    } else {
        oss << sentence.subject;
    }
    oss << "\" << std::endl;\n";

    if (!sentence.verb.empty() && (!sentence.adjectives.empty() || !sentence.object.empty())) {
        std::string verb = toLower(sentence.verb);
        if (verb == "was" || verb == "is" || verb == "became") {
            std::string normalizedSubject = normalizeIdentifier(stripArticle(sentence.subject));
            bool hasNegation = false;
            std::string stateStr;
            if (!sentence.adjectives.empty() && toLower(sentence.adjectives[0]) == "not" && !sentence.object.empty()) {
                hasNegation = true;
                stateStr = sentence.object;
            } else if (!sentence.adjectives.empty()) {
                for (size_t i = 0; i < sentence.adjectives.size(); i++) {
                    std::string adjective = toLower(sentence.adjectives[i]);
                    if (adjective == "not" && i + 1 < sentence.adjectives.size()) {
                        hasNegation = true;
                        stateStr = sentence.adjectives[i + 1];
                        break;
                    } else if (i == sentence.adjectives.size() - 1) {
                        stateStr = sentence.adjectives[i];
                    }
                }
            } else if (!sentence.object.empty()) {
                stateStr = sentence.object;
            }
            std::string normalizedState = normalizeIdentifier(stateStr);
            std::string stateVar = normalizedSubject + "_is_" + normalizedState;
            if (entityStates.find(stateVar) != entityStates.end()) {
                oss << indent << stateVar << " = " << (hasNegation ? "false" : "true") << ";\n";
            }
        }
    }
}

void CodeGenerator::handleRandomSentence(std::ostringstream& oss, const Sentence& sentence, int indentLevel) const {
    std::string indent(indentLevel * 4, ' ');
    
    if (!sentence.subject.empty() && !sentence.randomStates.first.empty() && !sentence.randomStates.second.empty()) {
        std::string normalizedSubject = normalizeIdentifier(stripArticle(sentence.subject));
        std::string normalizedState1 = normalizeIdentifier(sentence.randomStates.first);
        std::string normalizedState2 = normalizeIdentifier(sentence.randomStates.second);
        
        std::string stateVar1 = normalizedSubject + "_is_" + normalizedState1;
        std::string stateVar2 = normalizedSubject + "_is_" + normalizedState2;
        
        oss << indent << "// Randomly determine the state of " << sentence.subject << "\n";
        oss << indent << "bool randomChoice = getRandomBool();\n";
        oss << indent << stateVar1 << " = randomChoice;\n";
        oss << indent << stateVar2 << " = !randomChoice;\n";
        
        oss << indent << "std::cout << \"The " << sentence.subject << " was " 
            << "\" << (randomChoice ? \"" << sentence.randomStates.first 
            << "\" : \"" << sentence.randomStates.second << "\") << \".\" << std::endl;\n";
    } else {
        oss << indent << "// Insufficient information to generate random state\n";
        oss << indent << "std::cout << \"Random state not determined.\" << std::endl;\n";
    }
}

void CodeGenerator::debugPrintSentenceStructure(const Sentence& sentence, int level) const {
    std::string indent(level * 4, ' ');
    
    if (sentence.isConditional) {
        std::cout << indent << "CONDITIONAL: " << sentence.condition << std::endl;
        
        std::cout << indent << "THEN BRANCH (" << sentence.thenBranch.size() << " sentences):" << std::endl;
        for (const auto& thenSentence : sentence.thenBranch) {
            debugPrintSentenceStructure(thenSentence, level + 1);
        }
        
        if (!sentence.elseBranch.empty()) {
            std::cout << indent << "ELSE BRANCH (" << sentence.elseBranch.size() << " sentences):" << std::endl;
            for (const auto& elseSentence : sentence.elseBranch) {
                debugPrintSentenceStructure(elseSentence, level + 1);
            }
        }
    } else {
        std::cout << indent << "SENTENCE: Subject=\"" << sentence.subject 
                  << "\", Verb=\"" << sentence.verb << "\"";
        
        if (!sentence.adjectives.empty()) {
            std::cout << ", Adjectives=[";
            for (size_t i = 0; i < sentence.adjectives.size(); i++) {
                std::cout << "\"" << sentence.adjectives[i] << "\"";
                if (i < sentence.adjectives.size() - 1)
                    std::cout << ", ";
            }
            std::cout << "]";
        }
        
        if (!sentence.object.empty()) {
            std::cout << ", Object=\"" << sentence.object << "\"";
        }
        
        std::cout << std::endl;
    }
}

void CodeGenerator::debugPrintEntityStates() const {
    std::cout << "Entity States:" << std::endl;
    for (const auto& state : entityStates) {
        std::cout << "  " << state.first << " = " << (state.second ? "true" : "false") << std::endl;
    }
    std::cout << std::endl;
}

std::string CodeGenerator::translateCondition(const std::string& condition) const {
    std::istringstream iss(condition);
    std::string word;
    std::vector<std::string> words;
    
    while (iss >> word) {
        words.push_back(word);
    }
    
    if (words.size() >= 3) {
        std::string subject = words[0];
        int startIdx = 0;
        
        if (toLower(subject) == "the" && words.size() >= 4) {
            subject = words[1];
            startIdx = 1;
        }
        
        std::string verb = words[startIdx + 1];
        
        if (toLower(verb) == "was" || toLower(verb) == "is") {
            bool hasNegation = false;
            std::string state;
            
            if (words.size() > startIdx + 3 && toLower(words[startIdx + 2]) == "not") {
                hasNegation = true;
                state = words[startIdx + 3];
            } else {
                state = words[startIdx + 2];
            }
            
            std::string normalizedSubject = normalizeIdentifier(subject);
            std::string normalizedState = normalizeIdentifier(state);
            std::string stateVar = normalizedSubject + "_is_" + normalizedState;
            
            if (entityStates.find(stateVar) != entityStates.end()) {
                return hasNegation ? "!" + stateVar : stateVar;
            }
        }
    }
    
    return "true";
}

std::string CodeGenerator::normalizeIdentifier(const std::string& text) const {
    std::string result = stripArticle(text);
    
    std::string normalized;
    for (char c : result) {
        if (std::isalpha(c) || c == '_') {
            normalized += std::tolower(c);
        } else if (c == ' ') {
            normalized += '_';
        }
    }
    
    return normalized;
}

std::string CodeGenerator::stripArticle(const std::string& text) const {
    std::string lower = toLower(text);
    if (lower.substr(0, 4) == "the " && text.length() > 4) {
        return text.substr(4);
    } else if (lower.substr(0, 2) == "a " && text.length() > 2) {
        return text.substr(2);
    } else if (lower.substr(0, 3) == "an " && text.length() > 3) {
        return text.substr(3);
    }
    return text;
}

std::string CodeGenerator::toLower(const std::string& text) const {
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), 
                  [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string CodeGenerator::generateStatement(const Sentence& sentence) const {
    std::ostringstream oss;
    generateCodeForSentence(oss, sentence, 0);
    return oss.str();
}