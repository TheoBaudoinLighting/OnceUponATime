#include "pch.h"

#include "lexer.h"
#include "parser.h"
#include "code_generator.h"
#include "ast.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

static std::string readFile(const std::filesystem::path& path) {
    std::ifstream input(path);
    std::stringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

static std::string quotePath(const std::filesystem::path& path) {
    return "\"" + path.string() + "\"";
}

static int countOccurrences(const std::string& text, const std::string& needle) {
    int count = 0;
    size_t pos = 0;
    while ((pos = text.find(needle, pos)) != std::string::npos) {
        count++;
        pos += needle.size();
    }
    return count;
}

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

    EXPECT_NE(generated.find("std::vector<std::string> hero_companions = {\"Alice\", \"Bob\", \"Charlie\"};"), std::string::npos);
    EXPECT_NE(generated.find("for (const auto& companion : hero_companions)"), std::string::npos);

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "ouat_integration_test";
    std::filesystem::create_directories(tempDir);
    std::filesystem::path sourcePath = tempDir / "generated.cpp";
    std::filesystem::path exePath = tempDir / "generated.exe";
    std::filesystem::path compileLog = tempDir / "compile.log";
    std::filesystem::path outputLog = tempDir / "output.txt";

    {
        std::ofstream out(sourcePath);
        ASSERT_TRUE(out.good());
        out << generated;
    }

#ifdef _WIN32
    if (std::system("where cl >nul 2>nul") != 0) {
        SUCCEED() << "cl is not available in PATH; generated code shape was still verified.";
        return;
    }
    std::string compileCommand =
        "cl /nologo /EHsc /std:c++17 /Fe:" + quotePath(exePath) + " " +
        quotePath(sourcePath) + " > " + quotePath(compileLog) + " 2>&1";
#else
    if (std::system("command -v g++ >/dev/null 2>&1") != 0) {
        SUCCEED() << "g++ is not available; generated code shape was still verified.";
        return;
    }
    std::string compileCommand =
        "g++ -std=c++17 " + quotePath(sourcePath) + " -o " + quotePath(exePath) +
        " > " + quotePath(compileLog) + " 2>&1";
#endif

    ASSERT_EQ(std::system(compileCommand.c_str()), 0) << readFile(compileLog);

    std::string runCommand = quotePath(exePath) + " > " + quotePath(outputLog) + " 2>&1";
    ASSERT_EQ(std::system(runCommand.c_str()), 0) << readFile(outputLog);

    std::string output = readFile(outputLog);
    EXPECT_EQ(countOccurrences(output, "Hello"), 3);
}

TEST(IntegrationTest, GeneratePpmImageProgram) {
    std::string script =
        "Once upon a time. "
        "The image has width of 2 and height is 2. "
        "Create image canvas with width image width and height image height. "
        "For each y from 0 to image height do "
        "For each x from 0 to image width do "
        "Paint canvas at x y with 1 0 0. "
        "Endfor. "
        "Endfor. "
        "Save image canvas to \"ouat_integration_test/image.ppm\". "
        "The story ends.";

    Lexer lexer(script);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto story = parser.parseStory();
    ASSERT_NE(story, nullptr);

    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();

    EXPECT_NE(generated.find("struct OuatImage"), std::string::npos);
    EXPECT_NE(generated.find("paintPixel(canvas"), std::string::npos);
    EXPECT_NE(generated.find("saveImageAsPpm(canvas"), std::string::npos);

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "ouat_image_integration_test";
    std::filesystem::create_directories(tempDir);
    std::filesystem::path sourcePath = tempDir / "generated_image.cpp";
    std::filesystem::path exePath = tempDir / "generated_image.exe";
    std::filesystem::path compileLog = tempDir / "compile.log";
    std::filesystem::path outputLog = tempDir / "output.txt";

    {
        std::ofstream out(sourcePath);
        ASSERT_TRUE(out.good());
        out << generated;
    }

#ifdef _WIN32
    if (std::system("where cl >nul 2>nul") != 0) {
        SUCCEED() << "cl is not available in PATH; generated image code shape was still verified.";
        return;
    }
    std::string compileCommand =
        "cl /nologo /EHsc /std:c++17 /Fe:" + quotePath(exePath) + " " +
        quotePath(sourcePath) + " > " + quotePath(compileLog) + " 2>&1";
#else
    if (std::system("command -v g++ >/dev/null 2>&1") != 0) {
        SUCCEED() << "g++ is not available; generated image code shape was still verified.";
        return;
    }
    std::string compileCommand =
        "g++ -std=c++17 " + quotePath(sourcePath) + " -o " + quotePath(exePath) +
        " > " + quotePath(compileLog) + " 2>&1";
#endif

    ASSERT_EQ(std::system(compileCommand.c_str()), 0) << readFile(compileLog);

    std::string runCommand = quotePath(exePath) + " > " + quotePath(outputLog) + " 2>&1";
    ASSERT_EQ(std::system(runCommand.c_str()), 0) << readFile(outputLog);
}

TEST(IntegrationTest, GeneratePpmImageFromRecordFields) {
    std::string script =
        "Once upon a time. "
        "Define the record Color with red number and green number and blue number. "
        "The color is a Color with red 1 and green 0.5 and blue 0.25. "
        "Create image canvas with width 1 and height 1. "
        "Fill image canvas with color. "
        "Paint rectangle on canvas from 0 0 to 1 1 with color. "
        "Save image canvas to \"ouat_record_image_test/image.ppm\". "
        "The story ends.";

    Lexer lexer(script);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    auto story = parser.parseStory();
    ASSERT_NE(story, nullptr);

    CodeGeneratorVisitor codeGen;
    story->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();

    EXPECT_NE(generated.find("struct Color"), std::string::npos);
    EXPECT_NE(generated.find("Color color{};"), std::string::npos);
    EXPECT_NE(generated.find("color.red = 1;"), std::string::npos);
    EXPECT_NE(generated.find("fillImage(canvas, color.red, color.green, color.blue);"), std::string::npos);
    EXPECT_NE(generated.find("paintRectangle(canvas, static_cast<int>(0), static_cast<int>(0), static_cast<int>(1), static_cast<int>(1), color.red, color.green, color.blue);"), std::string::npos);

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "ouat_record_image_integration_test";
    std::filesystem::create_directories(tempDir);
    std::filesystem::path sourcePath = tempDir / "generated_record_image.cpp";
    std::filesystem::path exePath = tempDir / "generated_record_image.exe";
    std::filesystem::path compileLog = tempDir / "compile.log";
    std::filesystem::path outputLog = tempDir / "output.txt";

    {
        std::ofstream out(sourcePath);
        ASSERT_TRUE(out.good());
        out << generated;
    }

#ifdef _WIN32
    if (std::system("where cl >nul 2>nul") != 0) {
        SUCCEED() << "cl is not available in PATH; generated record image code shape was still verified.";
        return;
    }
    std::string compileCommand =
        "cl /nologo /EHsc /std:c++17 /Fe:" + quotePath(exePath) + " " +
        quotePath(sourcePath) + " > " + quotePath(compileLog) + " 2>&1";
#else
    if (std::system("command -v g++ >/dev/null 2>&1") != 0) {
        SUCCEED() << "g++ is not available; generated record image code shape was still verified.";
        return;
    }
    std::string compileCommand =
        "g++ -std=c++17 " + quotePath(sourcePath) + " -o " + quotePath(exePath) +
        " > " + quotePath(compileLog) + " 2>&1";
#endif

    ASSERT_EQ(std::system(compileCommand.c_str()), 0) << readFile(compileLog);

    std::string runCommand = quotePath(exePath) + " > " + quotePath(outputLog) + " 2>&1";
    ASSERT_EQ(std::system(runCommand.c_str()), 0) << readFile(outputLog);
}
