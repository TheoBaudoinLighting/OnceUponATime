// code_generator_tests.cpp

#include "pch.h"

#include "code_generator.h"
#include "ast.h"
#include <memory>
#include <string>
#include <vector>

TEST(CodeGeneratorTest, NarrativeGenerationTest) {
    AST::NarrativeStatement narrative("princess lived happily");
    CodeGeneratorVisitor codeGen;
    narrative.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("princess lived happily"), std::string::npos);
}

TEST(CodeGeneratorTest, ConditionalGenerationTest) {
    auto narrativeThen = std::make_unique<AST::NarrativeStatement>("door opens");
    AST::ConditionalStatement cond("door is unlocked");
    cond.thenBranch.push_back(std::move(narrativeThen));
    CodeGeneratorVisitor codeGen;
    cond.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("if ("), std::string::npos);
    EXPECT_NE(generated.find("getStoryState(\"door\") == \"unlocked\""), std::string::npos);
}

TEST(CodeGeneratorTest, WhileAndForEachGenerationTest) {
    auto narrativeWhile = std::make_unique<AST::NarrativeStatement>("dragon roars loudly");
    auto whileStmt = std::make_unique<AST::WhileStatement>("dragon is awake");
    whileStmt->body.push_back(std::move(narrativeWhile));

    auto narrativeForEach = std::make_unique<AST::NarrativeStatement>("companion joins bravely");
    auto foreachStmt = std::make_unique<AST::ForEachStatement>("companion", "squad");
    foreachStmt->body.push_back(std::move(narrativeForEach));

    CodeGeneratorVisitor codeGen;
    whileStmt->accept(codeGen);
    foreachStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("while ("), std::string::npos);
    EXPECT_NE(generated.find("for (const auto& companion : squad)"), std::string::npos);
}

TEST(CodeGeneratorTest, FunctionGenerationTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("hero heals quickly");
    auto funcDecl = std::make_unique<AST::FunctionDeclaration>("healHero");
    funcDecl->body.push_back(std::move(narrative));

    CodeGeneratorVisitor codeGen;
    funcDecl->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("void healHero()"), std::string::npos);
}

TEST(CodeGeneratorTest, VariableDeclarationGenerationTest) {
    AST::VariableDeclaration varDecl("The hero", "strength", "10");
    CodeGeneratorVisitor codeGen;
    varDecl.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int hero_strength = 10;"), std::string::npos);
}

TEST(CodeGeneratorTest, DecimalVariableDeclarationGenerationTest) {
    AST::VariableDeclaration varDecl("The lens", "radius", "0.5");
    CodeGeneratorVisitor codeGen;
    varDecl.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("double lens_radius = 0.5;"), std::string::npos);
}

TEST(CodeGeneratorTest, ArithmeticGenerationTest) {
    AST::Story story;
    auto vars = std::make_unique<AST::VariableDeclarationBlock>();
    vars->declarations.push_back(std::make_unique<AST::VariableDeclaration>("Magic", "value", "5"));
    story.statements.push_back(std::move(vars));
    story.statements.push_back(std::make_unique<AST::ArithmeticStatement>("Magic value", "subtract", "1", "Magic value"));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int magic = 5;"), std::string::npos);
    EXPECT_NE(generated.find("magic = magic - 1;"), std::string::npos);
}

TEST(CodeGeneratorTest, ImageRuntimeGenerationTest) {
    AST::Story story;
    auto vars = std::make_unique<AST::VariableDeclarationBlock>();
    vars->declarations.push_back(std::make_unique<AST::VariableDeclaration>("Image", "width", "4"));
    vars->declarations.push_back(std::make_unique<AST::VariableDeclaration>("Image", "height", "2"));
    story.statements.push_back(std::move(vars));
    story.statements.push_back(std::make_unique<AST::ImageDeclaration>("canvas", "image width", "image height"));
    story.statements.push_back(std::make_unique<AST::ImageFillStatement>("canvas", "0.1", "0.2", "0.3"));
    story.statements.push_back(std::make_unique<AST::RectanglePaintStatement>(
        "canvas", "0", "0", "2", "1", "1", "0", "0"));
    story.statements.push_back(std::make_unique<AST::PixelWriteStatement>("canvas", "0", "1", "1", "0.5", "0"));
    story.statements.push_back(std::make_unique<AST::ImageSaveStatement>("canvas", "output/gradient.ppm"));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("struct OuatImage"), std::string::npos);
    EXPECT_NE(generated.find("OuatImage canvas = makeImage(static_cast<int>(image_width), static_cast<int>(image_height));"), std::string::npos);
    EXPECT_NE(generated.find("fillImage(canvas, 0.1, 0.2, 0.3);"), std::string::npos);
    EXPECT_NE(generated.find("paintRectangle(canvas, static_cast<int>(0), static_cast<int>(0), static_cast<int>(2), static_cast<int>(1), 1, 0, 0);"), std::string::npos);
    EXPECT_NE(generated.find("paintPixel(canvas, static_cast<int>(0), static_cast<int>(1), 1, 0.5, 0);"), std::string::npos);
    EXPECT_NE(generated.find("saveImageAsPpm(canvas, \"output/gradient.ppm\");"), std::string::npos);
}

TEST(CodeGeneratorTest, RecordGenerationAndFieldAccessTest) {
    AST::Story story;
    story.statements.push_back(std::make_unique<AST::RecordDeclaration>(
        "Vec3",
        std::vector<std::pair<std::string, std::string>>{{"x", "number"}, {"y", "number"}, {"z", "number"}}));
    story.statements.push_back(std::make_unique<AST::RecordInstanceDeclaration>(
        "The color",
        "Vec3",
        std::vector<std::pair<std::string, std::string>>{{"x", "1"}, {"y", "0.5"}, {"z", "0.25"}}));
    story.statements.push_back(std::make_unique<AST::ImageDeclaration>("canvas", "1", "1"));
    story.statements.push_back(std::make_unique<AST::PixelWriteStatement>(
        "canvas", "0", "0", "color x", "color y", "color z"));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("struct Vec3"), std::string::npos);
    EXPECT_NE(generated.find("double x = 0;"), std::string::npos);
    EXPECT_NE(generated.find("Vec3 color{};"), std::string::npos);
    EXPECT_NE(generated.find("color.x = 1;"), std::string::npos);
    EXPECT_NE(generated.find("paintPixel(canvas, static_cast<int>(0), static_cast<int>(0), color.x, color.y, color.z);"), std::string::npos);
}

TEST(CodeGeneratorTest, NestedRecordFieldAccessTest) {
    AST::Story story;
    story.statements.push_back(std::make_unique<AST::RecordDeclaration>(
        "Vec3",
        std::vector<std::pair<std::string, std::string>>{{"x", "number"}, {"y", "number"}, {"z", "number"}}));
    story.statements.push_back(std::make_unique<AST::RecordDeclaration>(
        "Ray",
        std::vector<std::pair<std::string, std::string>>{{"origin", "Vec3"}, {"direction", "Vec3"}}));
    story.statements.push_back(std::make_unique<AST::RecordInstanceDeclaration>(
        "The camera origin",
        "Vec3",
        std::vector<std::pair<std::string, std::string>>{{"x", "0"}, {"y", "1"}, {"z", "2"}}));
    story.statements.push_back(std::make_unique<AST::RecordInstanceDeclaration>(
        "The camera ray",
        "Ray",
        std::vector<std::pair<std::string, std::string>>{{"origin", "camera origin"}, {"direction", "camera origin"}}));
    story.statements.push_back(std::make_unique<AST::ImageDeclaration>("canvas", "1", "1"));
    story.statements.push_back(std::make_unique<AST::PixelWriteStatement>(
        "canvas", "0", "0", "camera ray origin x", "camera ray origin y", "camera ray origin z"));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("struct Ray"), std::string::npos);
    EXPECT_NE(generated.find("Vec3 origin = {};"), std::string::npos);
    EXPECT_NE(generated.find("Ray camera_ray{};"), std::string::npos);
    EXPECT_NE(generated.find("camera_ray.origin = camera_origin;"), std::string::npos);
    EXPECT_NE(generated.find("paintPixel(canvas, static_cast<int>(0), static_cast<int>(0), camera_ray.origin.x, camera_ray.origin.y, camera_ray.origin.z);"), std::string::npos);
}

TEST(CodeGeneratorTest, CollectionDeclarationGenerationTest) {
    AST::VariableDeclaration collection("The hero", "companions", std::vector<std::string>{"Alice", "Bob", "Charlie"});
    CodeGeneratorVisitor codeGen;
    collection.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("std::vector<std::string> hero_companions = {\"Alice\", \"Bob\", \"Charlie\"};"), std::string::npos);
}

TEST(CodeGeneratorTest, StoryGenerationTest) {
    auto narrative = std::make_unique<AST::NarrativeStatement>("princess lived bravely");
    AST::Story story;
    story.statements.push_back(std::move(narrative));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int main()"), std::string::npos);
    EXPECT_NE(generated.find("princess lived bravely"), std::string::npos);
}

TEST(CodeGeneratorTest, StoryHoistsFunctionDeclarationsTest) {
    auto funcDecl = std::make_unique<AST::FunctionDeclaration>("healHero");
    funcDecl->body.push_back(std::make_unique<AST::TellStatement>("healed"));
    auto funcCall = std::make_unique<AST::FunctionCall>("healHero");

    AST::Story story;
    story.statements.push_back(std::move(funcDecl));
    story.statements.push_back(std::move(funcCall));

    CodeGeneratorVisitor codeGen;
    story.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    size_t functionPosition = generated.find("void healHero() {");
    size_t mainPosition = generated.find("int main()");
    ASSERT_NE(functionPosition, std::string::npos);
    ASSERT_NE(mainPosition, std::string::npos);
    EXPECT_LT(functionPosition, mainPosition);
}

TEST(CodeGeneratorTest, InteractiveGenerationTest) {
    AST::InteractiveStatement interactive("What is your name?");
    CodeGeneratorVisitor codeGen;
    interactive.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("std::cout <<"), std::string::npos);
    EXPECT_NE(generated.find("What is your name?"), std::string::npos);
    EXPECT_NE(generated.find("std::getline(std::cin, userInput0)"), std::string::npos);
}

TEST(CodeGeneratorTest, RandomGenerationTest) {
    AST::RandomStatement random("dragon", std::make_pair("friendly", "hostile"));
    CodeGeneratorVisitor codeGen;
    random.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("getRandomBool()"), std::string::npos);
    EXPECT_NE(generated.find("dragon_state"), std::string::npos);
    EXPECT_NE(generated.find("friendly"), std::string::npos);
    EXPECT_NE(generated.find("hostile"), std::string::npos);
}

TEST(CodeGeneratorTest, FunctionCallGenerationTest) {
    AST::FunctionCall funcCall("healHero");
    CodeGeneratorVisitor codeGen;
    funcCall.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("healHero();"), std::string::npos);
}

TEST(CodeGeneratorTest, ReturnStatementGenerationTest) {
    AST::ReturnStatement returnStmt;
    CodeGeneratorVisitor codeGen;
    returnStmt.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("return;"), std::string::npos);
}

TEST(CodeGeneratorTest, CommentGenerationTest) {
    AST::CommentStatement comment("This is a test comment");
    CodeGeneratorVisitor codeGen;
    comment.accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_EQ(generated.find("This is a test comment"), std::string::npos);
}

TEST(CodeGeneratorTest, VariableDeclarationBlockGenerationTest) {
    auto varDecl1 = std::make_unique<AST::VariableDeclaration>("The hero", "strength", "10");
    auto varDecl2 = std::make_unique<AST::VariableDeclaration>("The hero", "magic", "5");
    auto varBlock = std::make_unique<AST::VariableDeclarationBlock>();
    varBlock->declarations.push_back(std::move(varDecl1));
    varBlock->declarations.push_back(std::move(varDecl2));

    CodeGeneratorVisitor codeGen;
    varBlock->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("int hero_strength = 10;"), std::string::npos);
    EXPECT_NE(generated.find("int hero_magic = 5;"), std::string::npos);
}

TEST(CodeGeneratorTest, WhileMultipleStatementsGenerationTest) {
    auto whileStmt = std::make_unique<AST::WhileStatement>("dragon is awake");
    whileStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Hero trembles"));
    whileStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Knight prepares"));

    CodeGeneratorVisitor codeGen;
    whileStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("getStoryState(\"dragon\") == \"awake\""), std::string::npos);
    EXPECT_NE(generated.find("Hero trembles"), std::string::npos);
    EXPECT_NE(generated.find("Knight prepares"), std::string::npos);
}

TEST(CodeGeneratorTest, NestedWhileGenerationTest) {
    auto innerWhile = std::make_unique<AST::WhileStatement>("hero is brave");
    innerWhile->body.push_back(std::make_unique<AST::NarrativeStatement>("Hero fights"));

    auto outerWhile = std::make_unique<AST::WhileStatement>("dragon is awake");
    outerWhile->body.push_back(std::move(innerWhile));

    CodeGeneratorVisitor codeGen;
    outerWhile->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("getStoryState(\"dragon\") == \"awake\""), std::string::npos);
    EXPECT_NE(generated.find("getStoryState(\"hero\") == \"brave\""), std::string::npos);
    EXPECT_NE(generated.find("Hero fights"), std::string::npos);
}

TEST(CodeGeneratorTest, ForEachMultipleStatementsGenerationTest) {
    auto forEachStmt = std::make_unique<AST::ForEachStatement>("knight", "round table");
    forEachStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Knight stands"));
    forEachStmt->body.push_back(std::make_unique<AST::NarrativeStatement>("Knight bows"));

    CodeGeneratorVisitor codeGen;
    forEachStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (const auto& knight : round_table)"), std::string::npos);
    EXPECT_NE(generated.find("Knight stands"), std::string::npos);
    EXPECT_NE(generated.find("Knight bows"), std::string::npos);
}

TEST(CodeGeneratorTest, ForRangeGenerationTest) {
    auto forRangeStmt = std::make_unique<AST::ForRangeStatement>("y", "0", "height");
    forRangeStmt->body.push_back(std::make_unique<AST::TellStatement>("row"));

    CodeGeneratorVisitor codeGen;
    forRangeStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (int y = static_cast<int>(0); y < static_cast<int>("), std::string::npos);
    EXPECT_NE(generated.find("std::cout << \"row\""), std::string::npos);
}

TEST(CodeGeneratorTest, NestedForEachGenerationTest) {
    auto innerForEach = std::make_unique<AST::ForEachStatement>("room", "castle");
    innerForEach->body.push_back(std::make_unique<AST::NarrativeStatement>("Room is cleaned"));

    auto outerForEach = std::make_unique<AST::ForEachStatement>("castle", "kingdom");
    outerForEach->body.push_back(std::move(innerForEach));

    CodeGeneratorVisitor codeGen;
    outerForEach->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (const auto& castle : kingdom)"), std::string::npos);
    EXPECT_NE(generated.find("for (const auto& room : castle)"), std::string::npos);
    EXPECT_NE(generated.find("Room is cleaned"), std::string::npos);
}

TEST(CodeGeneratorTest, LoopWithConditionalGenerationTest) {
    auto conditional = std::make_unique<AST::ConditionalStatement>("knight is brave");
    conditional->thenBranch.push_back(std::make_unique<AST::NarrativeStatement>("Knight fights"));

    auto forEachStmt = std::make_unique<AST::ForEachStatement>("knight", "round table");
    forEachStmt->body.push_back(std::move(conditional));

    CodeGeneratorVisitor codeGen;
    forEachStmt->accept(codeGen);
    std::string generated = codeGen.getGeneratedCode();
    EXPECT_NE(generated.find("for (const auto& knight : round_table)"), std::string::npos);
    EXPECT_NE(generated.find("getStoryState(\"knight\") == \"brave\""), std::string::npos);
    EXPECT_NE(generated.find("Knight fights"), std::string::npos);
}
