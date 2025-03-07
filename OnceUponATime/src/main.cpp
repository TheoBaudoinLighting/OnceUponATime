// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <filesystem>
#include "lexer.hpp"
#include "parser.hpp"
#include "code_generator.hpp"

int main() {
    try {
        std::filesystem::path currentPath = std::filesystem::current_path();
        std::cout << "Current directory: " << currentPath.string() << std::endl;
        std::filesystem::path inputFilePath = currentPath / "../examples" / "dragon_tale.ouat";
        if (!std::filesystem::exists(inputFilePath)) {
            std::cerr << "Error: Input file " << inputFilePath.string() << " does not exist." << std::endl;
            return EXIT_FAILURE;
        }
        std::filesystem::path outputDirPath = currentPath / "output";
        std::filesystem::path outputFilePath = outputDirPath / "generated.cpp";
        std::filesystem::path exePath = outputDirPath / "generated.exe";
        std::cout << "Input file: " << inputFilePath.string() << std::endl;
        std::cout << "Output directory: " << outputDirPath.string() << std::endl;
        std::cout << "Generated file: " << outputFilePath.string() << std::endl;
        std::cout << "Executable: " << exePath.string() << std::endl;
        std::ifstream inputFile(inputFilePath);
        if (!inputFile) {
            std::cerr << "Error: Unable to open " << inputFilePath.string() << std::endl;
            return EXIT_FAILURE;
        }
        std::stringstream buffer;
        buffer << inputFile.rdbuf();
        std::string script = buffer.str();
        Lexer lexer(script);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto story = parser.parseStory();
        CodeGeneratorVisitor codeGen;
        story->accept(codeGen);
        std::string generatedCode = codeGen.getGeneratedCode();
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
        std::string compileCommand = "g++ \"" + outputFilePath.string() + "\" -std=c++17 -o \"" + exePath.string() + "\"";
        std::cout << "Compilation command: " << compileCommand << std::endl;
        std::cout << "Compiling generated code..." << std::endl;
        if (system(compileCommand.c_str()) != 0) {
            std::cerr << "Compilation failed." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Compilation successful: " << exePath.string() << " created." << std::endl;
        std::cout << "Running the generated executable..." << std::endl;
        std::string runCommand = "\"" + exePath.string() + "\"";
        if (system(runCommand.c_str()) != 0) {
            std::cerr << "Failed to run the executable." << std::endl;
            return EXIT_FAILURE;
        }
        std::cout << "Execution completed successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
