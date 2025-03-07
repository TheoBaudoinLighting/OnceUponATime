#include <gtest/gtest.h>
#include "../src/lexer.hpp"
#include "../src/parser.hpp"
#include "../src/code_generator.hpp"
#include "../src/ast.hpp"
#include <memory>
#include <string>
#include <fstream>
#include <cstdlib>

TEST(IntegrationTest, CompileAndRunGeneratedCode) {
    std::string script = 
        "Once upon a time. "
        "The hero has companions of [\"Alice\", \"Bob\", \"Charlie\"]. "
        "For each companion in hero companions do "
        "Tell \"Hello\". "
        "Endfor. "
        "The story ends.";
    
    Lexer lexer(script);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto story = parser.parseStory();
    ASSERT_NE(story, nullptr);
    
    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    
    std::string tempFile = "temp_generated.cpp";
    std::ofstream outFile(tempFile);
    outFile << generated;
    outFile.close();
    
    std::string compileCmd = "g++ " + tempFile + " -std=c++17 -o temp_generated.exe";
    int compileResult = std::system(compileCmd.c_str());
    ASSERT_EQ(compileResult, 0) << "La compilation a échoué";
    
#ifdef _WIN32
    std::string runCommand = "temp_generated.exe > temp_output.txt";
#else
    std::string runCommand = "./temp_generated.exe > temp_output.txt";
#endif

    int runResult = std::system(runCommand.c_str());
    ASSERT_EQ(runResult, 0) << "L'exécution a échoué";
    
    std::ifstream inFile("temp_output.txt");
    std::string line;
    int helloCount = 0;
    while (std::getline(inFile, line)) {
        if (line.find("Hello") != std::string::npos) {
            helloCount++;
        }
    }
    EXPECT_EQ(helloCount, 3) << "Le nombre de 'Hello' ne correspond pas au nombre de compagnons";
    
    std::remove(tempFile.c_str());
    std::remove("temp_generated.exe");
    std::remove("temp_output.txt");
}
