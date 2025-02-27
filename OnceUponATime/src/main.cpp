// main.cpp

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include "lexer.hpp"
#include "parser.hpp"
#include "code_generator.hpp"
#include "token.hpp"

int main() {
    try {
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << "Current working directory: " << currentPath.string() << std::endl;
        
        std::filesystem::path inputFilePath = currentPath / "../examples" / "dragon_tale.ouat";
        std::filesystem::path outputDirPath = currentPath / "output";
        std::filesystem::path outputFilePath = outputDirPath / "generated.cpp";
        std::filesystem::path exePath = outputDirPath / "generated.exe";
        
        std::cout << "Input file path: " << inputFilePath.string() << std::endl;
        std::cout << "Output directory: " << outputDirPath.string() << std::endl;
        std::cout << "Output file path: " << outputFilePath.string() << std::endl;
        std::cout << "Executable path: " << exePath.string() << std::endl;
        
        std::ifstream inputFile(inputFilePath);
        if (!inputFile) {
            std::cerr << "Error: Unable to open file " << inputFilePath.string() << std::endl;
            return EXIT_FAILURE;
        }
        std::stringstream buffer;
        buffer << inputFile.rdbuf();
        std::string script = buffer.str();

        Lexer lexer(script);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::vector<Sentence> sentences = parser.parseScript();

        std::cout << "Parsed sentence structure:" << std::endl;
        for (const auto& sentence : sentences) {
            if (sentence.isConditional) {
                std::cout << "Condition: " << sentence.condition << std::endl;
                std::cout << "Then branch size: " << sentence.thenBranch.size() << std::endl;
                std::cout << "Else branch size: " << sentence.elseBranch.size() << std::endl;
                
                if (sentence.elseBranch.size() == 1 && sentence.elseBranch[0].isConditional) {
                    std::cout << "  Else branch contains an 'else if' with condition: "
                              << sentence.elseBranch[0].condition << std::endl;
                    std::cout << "  Else-if then branch size: " 
                              << sentence.elseBranch[0].thenBranch.size() << std::endl;
                    std::cout << "  Else-if else branch size: " 
                              << sentence.elseBranch[0].elseBranch.size() << std::endl;
                }
            }
        }
        std::cout << std::endl;

        CodeGenerator codeGen(sentences);
        
        std::cout << "Entity states before code generation:" << std::endl;
        codeGen.debugPrintEntityStates();
        
        std::string generatedCode = codeGen.generateCode();
        
        std::cout << "Generated code:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << generatedCode << std::endl;
        std::cout << "----------------------------------------" << std::endl;

        std::filesystem::create_directories(outputDirPath);

        std::ofstream outFile(outputFilePath);
        if (!outFile) {
            std::cerr << "Error: Unable to open " << outputFilePath.string() << " for writing." << std::endl;
            return EXIT_FAILURE;
        }
        outFile << generatedCode;
        outFile.close();
        std::cout << "Generated code written to " << outputFilePath.string() << std::endl;

        std::string compileCommand = "g++ \"" + outputFilePath.string() + "\" -o \"" + exePath.string() + "\"";
        std::cout << "Compile command: " << compileCommand << std::endl;
        std::cout << "Compiling generated code..." << std::endl;
        if (system(compileCommand.c_str()) != 0) {
            std::cerr << "Compilation failed." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Compilation successful: " << exePath.string() << " created." << std::endl;

        std::cout << "Running generated executable..." << std::endl;
        std::string runCommand = "\"" + exePath.string() + "\"";
        std::cout << "Run command: " << runCommand << std::endl;
        if (system(runCommand.c_str()) != 0) {
            std::cerr << "Executable execution failed." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Execution completed successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}