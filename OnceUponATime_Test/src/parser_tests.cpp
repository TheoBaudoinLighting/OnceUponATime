// parser_tests.cpp

#include "pch.h"

#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include <memory>
#include <string>

std::unique_ptr<AST::Story> parseScript(const std::string &source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    Parser parser(tokens);
    return parser.parseStory();
}

TEST(ParserTest, PrologueEpilogueTest) {
    std::string script = "Once upon a time. The hero acted. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    EXPECT_GE(story->statements.size(), 1);
}

TEST(ParserTest, NarrativeSentenceTest) {
    std::string script = "Once upon a time. The knight fought bravely. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto narrative = dynamic_cast<AST::NarrativeStatement*>(story->statements[0].get());
    ASSERT_NE(narrative, nullptr);
    EXPECT_NE(narrative->text.find("The knight fought bravely"), std::string::npos);
}

TEST(ParserTest, ConditionalStatementTest) {
    std::string script = "Once upon a time. If hero is brave then The hero wins. End. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto conditional = dynamic_cast<AST::ConditionalStatement*>(story->statements[0].get());
    ASSERT_NE(conditional, nullptr);
    EXPECT_EQ(conditional->condition, "hero is brave");
    EXPECT_GE(conditional->thenBranch.size(), 1);
}

TEST(ParserTest, InteractiveStatementTest) {
    std::string script = "Once upon a time. Choose your next action carefully. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto interactive = dynamic_cast<AST::InteractiveStatement*>(story->statements[0].get());
    ASSERT_NE(interactive, nullptr);
    EXPECT_EQ(interactive->prompt, "your next action carefully");
}

TEST(ParserTest, RandomStatementTest) {
    std::string script = "Once upon a time. Random dragon leans towards friendly or hostile. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto random = dynamic_cast<AST::RandomStatement*>(story->statements[0].get());
    ASSERT_NE(random, nullptr);
    EXPECT_EQ(random->subject, "dragon");
    EXPECT_EQ(random->randomStates.first, "friendly");
    EXPECT_EQ(random->randomStates.second, "hostile");
}

TEST(ParserTest, WhileStatementTest) {
    std::string script = "Once upon a time. While dragon is awake. Hero trembles. Endwhile. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto whileStmt = dynamic_cast<AST::WhileStatement*>(story->statements[0].get());
    ASSERT_NE(whileStmt, nullptr);
    EXPECT_EQ(whileStmt->condition, "dragon is awake");
    EXPECT_GE(whileStmt->body.size(), 1);
}

TEST(ParserTest, ForEachStatementTest) {
    std::string script = "Once upon a time. For each knight in round table do Knight stands. Endfor. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto forEach = dynamic_cast<AST::ForEachStatement*>(story->statements[0].get());
    ASSERT_NE(forEach, nullptr);
    EXPECT_EQ(forEach->iterator, "knight");
    EXPECT_EQ(forEach->collection, "round table");
    EXPECT_GE(forEach->body.size(), 1);
}

TEST(ParserTest, ForRangeStatementTest) {
    std::string script = "Once upon a time. For each y from 0 to height do Tell \"row\". Endfor. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto forRange = dynamic_cast<AST::ForRangeStatement*>(story->statements[0].get());
    ASSERT_NE(forRange, nullptr);
    EXPECT_EQ(forRange->iterator, "y");
    EXPECT_EQ(forRange->start, "0");
    EXPECT_EQ(forRange->end, "height");
    EXPECT_EQ(forRange->body.size(), 1);
}

TEST(ParserTest, FunctionDeclarationTest) {
    std::string script = "Once upon a time. Define the function healHero as Hero recovers health. Endfunction. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(story->statements[0].get());
    ASSERT_NE(funcDecl, nullptr);
    EXPECT_EQ(funcDecl->name, "healHero");
    EXPECT_GE(funcDecl->body.size(), 1);
}

TEST(ParserTest, FunctionCallTest) {
    std::string script = "Once upon a time. Call healHero. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto funcCall = dynamic_cast<AST::FunctionCall*>(story->statements[0].get());
    ASSERT_NE(funcCall, nullptr);
    EXPECT_EQ(funcCall->name, "healHero");
}

TEST(ParserTest, VariableDeclarationTest) {
    std::string script = "Once upon a time. The hero has strength of 10. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto varBlock = dynamic_cast<AST::VariableDeclarationBlock*>(story->statements[0].get());
    ASSERT_NE(varBlock, nullptr);
    EXPECT_GE(varBlock->declarations.size(), 1);
    EXPECT_EQ(varBlock->declarations[0]->owner, "The hero");
    EXPECT_EQ(varBlock->declarations[0]->varName, "strength");
    EXPECT_EQ(varBlock->declarations[0]->value, "10");
}

TEST(ParserTest, ArithmeticStatementTest) {
    std::string script = "Once upon a time. Magic subtract 1 equals magic. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto arithmetic = dynamic_cast<AST::ArithmeticStatement*>(story->statements[0].get());
    ASSERT_NE(arithmetic, nullptr);
    EXPECT_EQ(arithmetic->left, "Magic");
    EXPECT_EQ(arithmetic->operation, "subtract");
    EXPECT_EQ(arithmetic->right, "1");
    EXPECT_EQ(arithmetic->target, "magic");
}

TEST(ParserTest, RecordDeclarationAndInstanceTest) {
    std::string script =
        "Once upon a time. "
        "Define the record Vec3 with x number and y number and z number. "
        "The color is a Vec3 with x 1 and y 0.5 and z 0.25. "
        "The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    ASSERT_EQ(story->statements.size(), 2);

    auto record = dynamic_cast<AST::RecordDeclaration*>(story->statements[0].get());
    ASSERT_NE(record, nullptr);
    EXPECT_EQ(record->name, "Vec3");
    ASSERT_EQ(record->fields.size(), 3);
    EXPECT_EQ(record->fields[0].first, "x");
    EXPECT_EQ(record->fields[0].second, "number");

    auto instance = dynamic_cast<AST::RecordInstanceDeclaration*>(story->statements[1].get());
    ASSERT_NE(instance, nullptr);
    EXPECT_EQ(instance->name, "The color");
    EXPECT_EQ(instance->typeName, "Vec3");
    ASSERT_EQ(instance->fieldValues.size(), 3);
    EXPECT_EQ(instance->fieldValues[1].first, "y");
    EXPECT_EQ(instance->fieldValues[1].second, "0.5");
}

TEST(ParserTest, ImageStatementsTest) {
    std::string script =
        "Once upon a time. "
        "Create image canvas with width image width and height image height. "
        "Fill image canvas with back wall. "
        "Paint rectangle on canvas from 0 0 to 4 4 with left wall. "
        "Paint canvas at x y with color x, color y, color z. "
        "Save image canvas to \"output/gradient.ppm\". "
        "The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    ASSERT_EQ(story->statements.size(), 5);

    auto imageDecl = dynamic_cast<AST::ImageDeclaration*>(story->statements[0].get());
    ASSERT_NE(imageDecl, nullptr);
    EXPECT_EQ(imageDecl->name, "canvas");
    EXPECT_EQ(imageDecl->width, "image width");
    EXPECT_EQ(imageDecl->height, "image height");

    auto imageFill = dynamic_cast<AST::ImageFillStatement*>(story->statements[1].get());
    ASSERT_NE(imageFill, nullptr);
    EXPECT_EQ(imageFill->imageName, "canvas");
    EXPECT_EQ(imageFill->red, "back wall red");
    EXPECT_EQ(imageFill->green, "back wall green");
    EXPECT_EQ(imageFill->blue, "back wall blue");

    auto rectanglePaint = dynamic_cast<AST::RectanglePaintStatement*>(story->statements[2].get());
    ASSERT_NE(rectanglePaint, nullptr);
    EXPECT_EQ(rectanglePaint->imageName, "canvas");
    EXPECT_EQ(rectanglePaint->left, "0");
    EXPECT_EQ(rectanglePaint->bottom, "0");
    EXPECT_EQ(rectanglePaint->right, "4");
    EXPECT_EQ(rectanglePaint->top, "4");
    EXPECT_EQ(rectanglePaint->red, "left wall red");

    auto pixelWrite = dynamic_cast<AST::PixelWriteStatement*>(story->statements[3].get());
    ASSERT_NE(pixelWrite, nullptr);
    EXPECT_EQ(pixelWrite->imageName, "canvas");
    EXPECT_EQ(pixelWrite->x, "x");
    EXPECT_EQ(pixelWrite->y, "y");
    EXPECT_EQ(pixelWrite->red, "color x");
    EXPECT_EQ(pixelWrite->green, "color y");
    EXPECT_EQ(pixelWrite->blue, "color z");

    auto imageSave = dynamic_cast<AST::ImageSaveStatement*>(story->statements[4].get());
    ASSERT_NE(imageSave, nullptr);
    EXPECT_EQ(imageSave->imageName, "canvas");
    EXPECT_EQ(imageSave->outputPath, "output/gradient.ppm");
}

TEST(ParserTest, CollectionDeclarationTest) {
    std::string script = "Once upon a time. The hero has companions of [\"Alice\", \"Bob\", \"Charlie\"]. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto varBlock = dynamic_cast<AST::VariableDeclarationBlock*>(story->statements[0].get());
    ASSERT_NE(varBlock, nullptr);
    ASSERT_EQ(varBlock->declarations.size(), 1);
    EXPECT_EQ(varBlock->declarations[0]->owner, "The hero");
    EXPECT_EQ(varBlock->declarations[0]->varName, "companions");
    ASSERT_TRUE(varBlock->declarations[0]->isCollection());
    EXPECT_EQ(varBlock->declarations[0]->values.size(), 3);
    EXPECT_EQ(varBlock->declarations[0]->values[0], "Alice");
}

TEST(ParserTest, DisplayAndNarrateStatementsTest) {
    std::string script = "Once upon a time. Display \"Hello\". Narrate \"A storm rises\". The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    ASSERT_EQ(story->statements.size(), 2);
    auto display = dynamic_cast<AST::TellStatement*>(story->statements[0].get());
    auto narrate = dynamic_cast<AST::TellStatement*>(story->statements[1].get());
    ASSERT_NE(display, nullptr);
    ASSERT_NE(narrate, nullptr);
    EXPECT_EQ(display->message, "Hello");
    EXPECT_EQ(narrate->message, "A storm rises");
}

TEST(ParserTest, ElseIfUsesSingleEndifTest) {
    std::string script = "Once upon a time. If dragon is awake then Tell \"Careful\". Else if dragon is friendly then Tell \"Wave\". Endif. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto conditional = dynamic_cast<AST::ConditionalStatement*>(story->statements[0].get());
    ASSERT_NE(conditional, nullptr);
    ASSERT_EQ(conditional->elseBranch.size(), 1);
    auto elseIf = dynamic_cast<AST::ConditionalStatement*>(conditional->elseBranch[0].get());
    ASSERT_NE(elseIf, nullptr);
    EXPECT_EQ(elseIf->condition, "dragon is friendly");
}

TEST(ParserTest, WhileStatementMultipleBodyTest) {
    std::string script = "Once upon a time. While dragon is awake. Hero trembles. Knight prepares. Wizard casts spell. Endwhile. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto whileStmt = dynamic_cast<AST::WhileStatement*>(story->statements[0].get());
    ASSERT_NE(whileStmt, nullptr);
    EXPECT_EQ(whileStmt->condition, "dragon is awake");
    EXPECT_EQ(whileStmt->body.size(), 3);
    
    auto narrative1 = dynamic_cast<AST::NarrativeStatement*>(whileStmt->body[0].get());
    ASSERT_NE(narrative1, nullptr);
    EXPECT_EQ(narrative1->text, "Hero trembles");
    
    auto narrative2 = dynamic_cast<AST::NarrativeStatement*>(whileStmt->body[1].get());
    ASSERT_NE(narrative2, nullptr);
    EXPECT_EQ(narrative2->text, "Knight prepares");
}

TEST(ParserTest, NestedWhileStatementTest) {
    std::string script = "Once upon a time. While dragon is awake. While hero is brave. Hero fights. Endwhile. Endwhile. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto outerWhile = dynamic_cast<AST::WhileStatement*>(story->statements[0].get());
    ASSERT_NE(outerWhile, nullptr);
    EXPECT_EQ(outerWhile->condition, "dragon is awake");
    EXPECT_EQ(outerWhile->body.size(), 1);
    
    auto innerWhile = dynamic_cast<AST::WhileStatement*>(outerWhile->body[0].get());
    ASSERT_NE(innerWhile, nullptr);
    EXPECT_EQ(innerWhile->condition, "hero is brave");
    EXPECT_EQ(innerWhile->body.size(), 1);
}

TEST(ParserTest, ForEachStatementMultipleBodyTest) {
    std::string script = "Once upon a time. For each knight in round table do Knight stands. Knight bows. Knight sits. Endfor. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto forEach = dynamic_cast<AST::ForEachStatement*>(story->statements[0].get());
    ASSERT_NE(forEach, nullptr);
    EXPECT_EQ(forEach->iterator, "knight");
    EXPECT_EQ(forEach->collection, "round table");
    EXPECT_EQ(forEach->body.size(), 3); 
    
    auto narrative1 = dynamic_cast<AST::NarrativeStatement*>(forEach->body[0].get());
    ASSERT_NE(narrative1, nullptr);
    EXPECT_EQ(narrative1->text, "Knight stands");
    
    auto narrative2 = dynamic_cast<AST::NarrativeStatement*>(forEach->body[1].get());
    ASSERT_NE(narrative2, nullptr);
    EXPECT_EQ(narrative2->text, "Knight bows");
}

TEST(ParserTest, NestedForEachStatementTest) {
    std::string script = "Once upon a time. For each castle in kingdom do For each room in castle do Room is cleaned. Endfor. Endfor. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto outerForEach = dynamic_cast<AST::ForEachStatement*>(story->statements[0].get());
    ASSERT_NE(outerForEach, nullptr);
    EXPECT_EQ(outerForEach->iterator, "castle");
    EXPECT_EQ(outerForEach->collection, "kingdom");
    EXPECT_EQ(outerForEach->body.size(), 1);
    
    auto innerForEach = dynamic_cast<AST::ForEachStatement*>(outerForEach->body[0].get());
    ASSERT_NE(innerForEach, nullptr);
    EXPECT_EQ(innerForEach->iterator, "room");
    EXPECT_EQ(innerForEach->collection, "castle");
    EXPECT_EQ(innerForEach->body.size(), 1);
}

TEST(ParserTest, ForEachWithConditionalTest) {
    std::string script = "Once upon a time. For each knight in round table do If knight is brave then Knight fights. End. Endfor. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto forEach = dynamic_cast<AST::ForEachStatement*>(story->statements[0].get());
    ASSERT_NE(forEach, nullptr);
    EXPECT_EQ(forEach->iterator, "knight");
    EXPECT_EQ(forEach->collection, "round table");
    EXPECT_EQ(forEach->body.size(), 1);
    
    auto conditional = dynamic_cast<AST::ConditionalStatement*>(forEach->body[0].get());
    ASSERT_NE(conditional, nullptr);
    EXPECT_EQ(conditional->condition, "knight is brave");
    EXPECT_EQ(conditional->thenBranch.size(), 1);
}

TEST(ParserTest, WhileWithConditionalTest) {
    std::string script = "Once upon a time. While dragon is awake. If hero is brave then Hero fights. End. Endwhile. The story ends.";
    auto story = parseScript(script);
    ASSERT_NE(story, nullptr);
    auto whileStmt = dynamic_cast<AST::WhileStatement*>(story->statements[0].get());
    ASSERT_NE(whileStmt, nullptr);
    EXPECT_EQ(whileStmt->condition, "dragon is awake");
    EXPECT_EQ(whileStmt->body.size(), 1);
    
    auto conditional = dynamic_cast<AST::ConditionalStatement*>(whileStmt->body[0].get());
    ASSERT_NE(conditional, nullptr);
    EXPECT_EQ(conditional->condition, "hero is brave");
    EXPECT_EQ(conditional->thenBranch.size(), 1);
}
