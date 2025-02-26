/**
 * @file main.cpp
 * @author Théo Baudoin
 * @brief Main entry point for the Once Upon a Time (.ouat) compiler
 * @date 2025-02-26
 * 
 * This file contains the main function that coordinates the lexical
 * and syntactic analysis of .ouat source files.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"

int main() {
    try {
        // Read the test.ouat file (ensure correct path)
        std::ifstream inputFile("../examples/test.ouat");
        if (!inputFile) {
            std::cerr << "Error: Unable to open file test.ouat" << std::endl;
            return EXIT_FAILURE;
        }
        std::stringstream buffer;
        buffer << inputFile.rdbuf();
        std::string script = buffer.str();

        // Perform lexical analysis
        Lexer lexer(script);
        std::vector<Token> tokens = lexer.tokenize();

        // Display token list for verification
        std::cout << "Token list:" << std::endl;
        for (const auto& token : tokens) {
            std::cout << tokenTypeToString(token.type)
                      << "('" << token.lexeme << "') at line " << token.line
                      << ", column " << token.column << std::endl;
        }
        std::cout << std::endl;

        // Perform syntactic analysis
        Parser parser(tokens);
        std::vector<Sentence> sentences = parser.parseScript();

        // Display parsed sentences
        std::cout << "Parsed sentences:" << std::endl;
        for (const auto& sentence : sentences) {
            std::cout << "Subject: " << sentence.subject 
                      << ", Verb: " << sentence.verb;
            if (!sentence.adjectives.empty()) {
                std::cout << ", Adjectives: ";
                for (const auto& adj : sentence.adjectives) {
                    std::cout << adj << " ";
                }
            }
            if (!sentence.object.empty()) {
                std::cout << ", Object: " << sentence.object;
            }
            std::cout << std::endl;
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
