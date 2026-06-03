// code_generator.cpp
#include "code_generator.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>

static bool isNumberLiteral(const std::string& value) {
    if (value.empty()) {
        return false;
    }
    size_t start = value[0] == '-' ? 1 : 0;
    if (start == value.size()) {
        return false;
    }
    bool seenDecimalPoint = false;
    for (size_t i = start; i < value.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(value[i]);
        if (value[i] == '.' && !seenDecimalPoint) {
            seenDecimalPoint = true;
            continue;
        }
        if (!std::isdigit(c)) {
            return false;
        }
    }
    return true;
}

static std::vector<std::string> splitWords(const std::string& value) {
    std::istringstream iss(value);
    std::vector<std::string> words;
    std::string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}

static std::string joinWords(const std::vector<std::string>& words, size_t start, size_t end) {
    if (end > words.size()) {
        end = words.size();
    }
    std::ostringstream oss;
    for (size_t i = start; i < end; ++i) {
        if (i > start) {
            oss << " ";
        }
        oss << words[i];
    }
    return oss.str();
}

CodeGeneratorVisitor::CodeGeneratorVisitor()
    : indentLevel(0),
      skipFunctionDeclarations(false),
      skipRecordDeclarations(false),
      imageRuntimeRequired(false),
      tempCounter(0) {}

std::string CodeGeneratorVisitor::indent() const {
    return std::string(indentLevel * 4, ' ');
}

void CodeGeneratorVisitor::generateRandomizer() {
    oss << "bool getRandomBool() {\n";
    oss << "    return std::rand() % 2 == 0;\n";
    oss << "}\n\n";
}

void CodeGeneratorVisitor::generateStoryStateHelpers() {
    oss << "std::unordered_map<std::string, std::string> storyStates;\n\n";
    oss << "std::string getStoryState(const std::string& name) {\n";
    oss << "    auto it = storyStates.find(name);\n";
    oss << "    return it == storyStates.end() ? \"\" : it->second;\n";
    oss << "}\n\n";
    oss << "double getStoryNumber(const std::string& name) {\n";
    oss << "    try {\n";
    oss << "        return std::stod(getStoryState(name));\n";
    oss << "    } catch (...) {\n";
    oss << "        return 0;\n";
    oss << "    }\n";
    oss << "}\n\n";
    oss << "bool storyCondition(const std::string& condition) {\n";
    oss << "    return getStoryState(condition) == \"true\";\n";
    oss << "}\n\n";
}

void CodeGeneratorVisitor::generateImageRuntime() {
    oss << R"cpp(
struct OuatPixel {
    double red;
    double green;
    double blue;
};

struct OuatImage {
    int width;
    int height;
    std::vector<OuatPixel> pixels;
};

double ouatClamp(double value, double low, double high) {
    return std::max(low, std::min(value, high));
}

OuatImage makeImage(int width, int height) {
    width = std::max(1, width);
    height = std::max(1, height);
    return OuatImage{width, height, std::vector<OuatPixel>(static_cast<size_t>(width * height), OuatPixel{0, 0, 0})};
}

void paintPixel(OuatImage& image, int x, int y, double red, double green, double blue) {
    if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
        return;
    }
    image.pixels[static_cast<size_t>(y * image.width + x)] = OuatPixel{red, green, blue};
}

void fillImage(OuatImage& image, double red, double green, double blue) {
    for (auto& pixel : image.pixels) {
        pixel = OuatPixel{red, green, blue};
    }
}

void paintRectangle(OuatImage& image, int left, int bottom, int right, int top,
                    double red, double green, double blue) {
    if (right < left) {
        std::swap(left, right);
    }
    if (top < bottom) {
        std::swap(top, bottom);
    }

    left = std::max(0, std::min(left, image.width));
    right = std::max(0, std::min(right, image.width));
    bottom = std::max(0, std::min(bottom, image.height));
    top = std::max(0, std::min(top, image.height));

    for (int y = bottom; y < top; ++y) {
        for (int x = left; x < right; ++x) {
            paintPixel(image, x, y, red, green, blue);
        }
    }
}

int ouatColorByte(double value) {
    return static_cast<int>(255.999 * ouatClamp(value, 0.0, 1.0));
}

void saveImageAsPpm(const OuatImage& image, const std::string& outputPath) {
    std::filesystem::path path(outputPath);
    if (!path.parent_path().empty()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream output(path);
    if (!output) {
        std::cerr << "Unable to write image: " << outputPath << std::endl;
        return;
    }

    output << "P3\n" << image.width << " " << image.height << "\n255\n";
    for (int y = image.height - 1; y >= 0; --y) {
        for (int x = 0; x < image.width; ++x) {
            const OuatPixel& pixel = image.pixels[static_cast<size_t>(y * image.width + x)];
            output << ouatColorByte(pixel.red) << " "
                   << ouatColorByte(pixel.green) << " "
                   << ouatColorByte(pixel.blue) << "\n";
        }
    }
}

)cpp";
}

std::string CodeGeneratorVisitor::getGeneratedCode() const {
    return oss.str();
}

std::string CodeGeneratorVisitor::escapeString(const std::string& s) const {
    std::string result;
    for (char c : s) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result.push_back(c); break;
        }
    }
    return result;
}

std::string CodeGeneratorVisitor::sanitizeIdentifier(const std::string& s) const {
    std::vector<std::string> words;
    std::string current;
    for (char c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            current.push_back(static_cast<char>(std::tolower(uc)));
        } else if (!current.empty()) {
            words.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        words.push_back(current);
    }

    while (!words.empty() && (words.front() == "the" || words.front() == "a" || words.front() == "an")) {
        words.erase(words.begin());
    }
    if (words.empty()) {
        return "value";
    }

    std::ostringstream ossId;
    for (size_t i = 0; i < words.size(); ++i) {
        if (i > 0) {
            ossId << "_";
        }
        ossId << words[i];
    }
    std::string result = ossId.str();
    if (!result.empty() && std::isdigit(static_cast<unsigned char>(result.front()))) {
        result.insert(result.begin(), '_');
    }
    return result;
}

std::string CodeGeneratorVisitor::sanitizeTypeName(const std::string& s) const {
    std::vector<std::string> words;
    std::string current;
    for (char c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            current.push_back(c);
        } else if (!current.empty()) {
            words.push_back(current);
            current.clear();
        }
    }
    if (!current.empty()) {
        words.push_back(current);
    }
    if (words.empty()) {
        return "OuatRecord";
    }

    std::ostringstream typeName;
    for (auto word : words) {
        word[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(word[0])));
        typeName << word;
    }
    std::string result = typeName.str();
    if (!result.empty() && std::isdigit(static_cast<unsigned char>(result.front()))) {
        result = "Ouat" + result;
    }
    return result;
}

std::string CodeGeneratorVisitor::normalizeName(const std::string& s) const {
    return sanitizeIdentifier(s);
}

std::string CodeGeneratorVisitor::variableNameFor(const AST::VariableDeclaration& node) const {
    std::string owner = sanitizeIdentifier(node.owner);
    std::string variable = sanitizeIdentifier(node.varName);
    if (node.isCollection() &&
        (variable == "members" || variable == "member" ||
         variable == "items" || variable == "item" ||
         variable == "elements" || variable == "element")) {
        return owner;
    }
    if (owner.empty() || owner == "value") {
        return variable;
    }
    if (variable.empty() || variable == "value" || owner == variable) {
        return owner;
    }
    return owner + "_" + variable;
}

std::string CodeGeneratorVisitor::variableNameFor(const AST::RecordInstanceDeclaration& node) const {
    return sanitizeIdentifier(node.name);
}

std::string CodeGeneratorVisitor::resolveName(const std::string& name) const {
    std::string normalized = normalizeName(name);
    auto it = symbols.find(normalized);
    if (it != symbols.end()) {
        return it->second;
    }
    return "";
}

std::string CodeGeneratorVisitor::translateCondition(const std::string& condition) const {
    std::vector<std::string> words = splitWords(condition);
    if (words.empty()) {
        return "false";
    }

    auto conditionKey = normalizeName(condition);
    auto makeStateLookup = [&](const std::string& name) {
        return "getStoryState(\"" + escapeString(normalizeName(name)) + "\")";
    };
    auto makeNumberLookup = [&](const std::string& name) {
        return "getStoryNumber(\"" + escapeString(normalizeName(name)) + "\")";
    };

    for (size_t i = 0; i < words.size(); ++i) {
        std::string op = normalizeName(words[i]);
        if (op != "is" && op != "equals") {
            continue;
        }

        std::string leftRaw = joinWords(words, 0, i);
        if (leftRaw.empty()) {
            return "false";
        }

        size_t rhsStart = i + 1;
        std::string cppOp = "==";
        if (op == "is" && rhsStart < words.size()) {
            std::string first = normalizeName(words[rhsStart]);
            std::string second = rhsStart + 1 < words.size() ? normalizeName(words[rhsStart + 1]) : "";
            if (first == "not") {
                cppOp = "!=";
                rhsStart++;
            } else if (first == "greater" && second == "than") {
                cppOp = ">";
                rhsStart += 2;
            } else if (first == "less" && second == "than") {
                cppOp = "<";
                rhsStart += 2;
            } else if (first == "at" && second == "least") {
                cppOp = ">=";
                rhsStart += 2;
            } else if (first == "at" && second == "most") {
                cppOp = "<=";
                rhsStart += 2;
            }
        }

        std::string rightRaw = joinWords(words, rhsStart, words.size());
        if (rightRaw.empty()) {
            return "false";
        }

        std::string leftId = resolveName(leftRaw);
        std::string rightId = resolveName(rightRaw);
        bool numericComparison = cppOp == ">" || cppOp == "<" || cppOp == ">=" || cppOp == "<=" ||
                                 isNumberLiteral(rightRaw);

        if (numericComparison) {
            std::string leftExpr = leftId.empty() ? makeNumberLookup(leftRaw) : leftId;
            std::string rightExpr = rightId.empty() ? rightRaw : rightId;
            return leftExpr + " " + cppOp + " " + rightExpr;
        }

        std::string leftKind = leftId.empty() ? "" : symbolKinds.at(leftId);
        if (!leftId.empty() && leftKind == "string") {
            std::string rightExpr = rightId.empty()
                ? "\"" + escapeString(rightRaw) + "\""
                : rightId;
            return leftId + " " + cppOp + " " + rightExpr;
        }

        std::string leftExpr = leftId.empty() ? makeStateLookup(leftRaw) : leftId;
        std::string rightExpr = rightId.empty()
            ? "\"" + escapeString(rightRaw) + "\""
            : rightId;
        return leftExpr + " " + cppOp + " " + rightExpr;
    }

    return "storyCondition(\"" + escapeString(conditionKey) + "\")";
}

std::string CodeGeneratorVisitor::numericExpression(const std::string& value) const {
    if (isNumberLiteral(value)) {
        return value;
    }
    std::string id = resolveName(value);
    if (!id.empty()) {
        return id;
    }
    return "getStoryNumber(\"" + escapeString(normalizeName(value)) + "\")";
}

std::string CodeGeneratorVisitor::cppTypeFor(const std::string& typeName) const {
    std::string normalized = normalizeName(typeName);
    if (normalized == "number" || normalized == "numeric" ||
        normalized == "decimal" || normalized == "double" || normalized == "float") {
        return "double";
    }
    if (normalized == "integer" || normalized == "int") {
        return "int";
    }
    if (normalized == "text" || normalized == "string" || normalized == "word") {
        return "std::string";
    }
    if (normalized == "truth" || normalized == "boolean" || normalized == "bool") {
        return "bool";
    }

    auto it = recordTypeAliases.find(normalized);
    if (it != recordTypeAliases.end()) {
        return it->second;
    }
    return sanitizeTypeName(typeName);
}

std::string CodeGeneratorVisitor::cppDefaultValueFor(const std::string& typeName) const {
    std::string cppType = cppTypeFor(typeName);
    if (cppType == "std::string") {
        return "\"\"";
    }
    if (cppType == "bool") {
        return "false";
    }
    if (cppType == "int" || cppType == "double") {
        return "0";
    }
    return "{}";
}

std::string CodeGeneratorVisitor::kindForType(const std::string& typeName) const {
    std::string cppType = cppTypeFor(typeName);
    if (cppType == "std::string") {
        return "string";
    }
    if (cppType == "bool") {
        return "bool";
    }
    if (cppType == "int" || cppType == "double") {
        return "number";
    }
    return "record:" + cppType;
}

std::string CodeGeneratorVisitor::fieldTypeFor(const std::string& recordType, const std::string& fieldName) const {
    auto it = recordTypes.find(recordType);
    if (it == recordTypes.end()) {
        return "";
    }
    std::string normalizedField = normalizeName(fieldName);
    for (const auto& field : it->second) {
        if (normalizeName(field.sourceName) == normalizedField || normalizeName(field.cppName) == normalizedField) {
            return field.typeName;
        }
    }
    return "";
}

std::string CodeGeneratorVisitor::typedExpression(const std::string& value, const std::string& typeName) const {
    std::string cppType = cppTypeFor(typeName);
    std::string resolved = resolveName(value);

    if (cppType == "double" || cppType == "int") {
        return numericExpression(value);
    }
    if (cppType == "std::string") {
        if (!resolved.empty()) {
            return resolved;
        }
        return "\"" + escapeString(value) + "\"";
    }
    if (cppType == "bool") {
        std::string normalized = normalizeName(value);
        if (normalized == "true" || normalized == "yes" || normalized == "right") {
            return "true";
        }
        if (normalized == "false" || normalized == "no" || normalized == "wrong") {
            return "false";
        }
        if (!resolved.empty()) {
            return resolved;
        }
        return "false";
    }
    if (!resolved.empty()) {
        return resolved;
    }
    return sanitizeIdentifier(value);
}

void CodeGeneratorVisitor::registerDeclaration(const AST::VariableDeclaration& node) {
    std::string id = variableNameFor(node);
    std::string kind = node.isCollection() ? "collection" : (isNumberLiteral(node.value) ? "number" : "string");
    symbolKinds[id] = kind;

    symbols[normalizeName(node.owner + " " + node.varName)] = id;
    symbols[normalizeName(node.varName)] = id;
    if (node.varName == "state" || node.isCollection()) {
        symbols[normalizeName(node.owner)] = id;
    }
    if (node.isCollection()) {
        declaredCollections.insert(id);
    }
}

void CodeGeneratorVisitor::registerArithmeticTarget(const AST::ArithmeticStatement& node) {
    std::string normalized = normalizeName(node.target);
    if (symbols.find(normalized) != symbols.end()) {
        return;
    }
    std::string id = sanitizeIdentifier(node.target);
    symbols[normalized] = id;
    symbolKinds[id] = "number";
}

void CodeGeneratorVisitor::registerRecordType(const AST::RecordDeclaration& node) {
    std::string typeId = sanitizeTypeName(node.name);
    recordTypeAliases[normalizeName(node.name)] = typeId;

    std::vector<RecordField> fields;
    for (const auto& field : node.fields) {
        fields.push_back(RecordField{field.first, sanitizeIdentifier(field.first), field.second});
    }
    recordTypes[typeId] = fields;
}

void CodeGeneratorVisitor::registerRecordInstance(const AST::RecordInstanceDeclaration& node) {
    std::string id = variableNameFor(node);
    std::string typeId = cppTypeFor(node.typeName);
    symbols[normalizeName(node.name)] = id;
    symbolKinds[id] = "record:" + typeId;

    registerRecordFieldSymbols(node.name, id, typeId, 0);
}

void CodeGeneratorVisitor::registerRecordFieldSymbols(const std::string& sourcePrefix,
                                                       const std::string& cppPrefix,
                                                       const std::string& recordType,
                                                       int depth) {
    if (depth > 4) {
        return;
    }

    auto typeIt = recordTypes.find(recordType);
    if (typeIt == recordTypes.end()) {
        return;
    }
    for (const auto& field : typeIt->second) {
        std::string fieldAccess = cppPrefix + "." + field.cppName;
        symbols[normalizeName(sourcePrefix + " " + field.sourceName)] = fieldAccess;
        symbolKinds[fieldAccess] = kindForType(field.typeName);

        std::string nestedType = cppTypeFor(field.typeName);
        if (recordTypes.find(nestedType) != recordTypes.end()) {
            registerRecordFieldSymbols(sourcePrefix + " " + field.sourceName, fieldAccess, nestedType, depth + 1);
        }
    }
}

void CodeGeneratorVisitor::visit(AST::NarrativeStatement& node) {
    oss << indent() << "std::cout << \"" << escapeString(node.text) << "\" << std::endl;\n";
}

void CodeGeneratorVisitor::visit(AST::ConditionalStatement& node) {
    oss << indent() << "if (" << translateCondition(node.condition) << ") {\n";
    indentLevel++;
    for (auto& stmt : node.thenBranch) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}";
    if (!node.elseBranch.empty()) {
        oss << " else {\n";
        indentLevel++;
        for (auto& stmt : node.elseBranch) {
            stmt->accept(*this);
        }
        indentLevel--;
        oss << indent() << "}\n";
    } else {
        oss << "\n";
    }
}

void CodeGeneratorVisitor::visit(AST::InteractiveStatement& node) {
    int inputId = tempCounter++;
    oss << indent() << "{\n";
    indentLevel++;
    oss << indent() << "std::cout << \"" << escapeString(node.prompt) << " \";\n";
    oss << indent() << "std::string userInput" << inputId << ";\n";
    oss << indent() << "std::getline(std::cin, userInput" << inputId << ");\n";
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::RandomStatement& node) {
    std::string subjectKey = normalizeName(node.subject);
    std::string stateName = sanitizeIdentifier(node.subject + " state random");
    oss << indent() << "bool randomChoice = getRandomBool();\n";
    oss << indent() << "std::string " << stateName << " = randomChoice ? \""
        << escapeString(node.randomStates.first) << "\" : \""
        << escapeString(node.randomStates.second) << "\";\n";
    oss << indent() << "storyStates[\"" << escapeString(subjectKey) << "\"] = " << stateName << ";\n";
    oss << indent() << "std::cout << \"The " << escapeString(node.subject) << " was \" << "
        << stateName << " << \".\" << std::endl;\n";
}

void CodeGeneratorVisitor::visit(AST::WhileStatement& node) {
    oss << indent() << "while (" << translateCondition(node.condition) << ") {\n";
    indentLevel++;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::ForEachStatement& node) {
    std::string collectionName = resolveName(node.collection);
    if (collectionName.empty()) {
        collectionName = sanitizeIdentifier(node.collection);
    }
    std::string iteratorName = sanitizeIdentifier(node.iterator);
    collectionsUsed.insert(collectionName);
    oss << indent() << "for (const auto& " << iteratorName << " : " << collectionName << ") {\n";
    indentLevel++;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::ForRangeStatement& node) {
    std::string iteratorName = sanitizeIdentifier(node.iterator);
    std::string startExpr = numericExpression(node.start);
    std::string endExpr = numericExpression(node.end);
    oss << indent() << "for (int " << iteratorName << " = static_cast<int>(" << startExpr
        << "); " << iteratorName << " < static_cast<int>(" << endExpr << "); ++"
        << iteratorName << ") {\n";
    indentLevel++;
    initializedSymbols.insert(iteratorName);
    symbols[normalizeName(node.iterator)] = iteratorName;
    symbolKinds[iteratorName] = "number";
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    indentLevel--;
    oss << indent() << "}\n";
}

void CodeGeneratorVisitor::visit(AST::FunctionDeclaration& node) {
    if (skipFunctionDeclarations) {
        return;
    }

    oss << "void " << node.name << "() {\n";
    indentLevel++;
    bool previousSkip = skipFunctionDeclarations;
    skipFunctionDeclarations = true;
    for (auto& stmt : node.body) {
        stmt->accept(*this);
    }
    skipFunctionDeclarations = previousSkip;
    indentLevel--;
    oss << "}\n\n";
}

void CodeGeneratorVisitor::visit(AST::FunctionCall& node) {
    oss << indent() << node.name << "();\n";
}

void CodeGeneratorVisitor::visit(AST::ReturnStatement& node) {
    oss << indent() << "return;\n";
}

void CodeGeneratorVisitor::visit(AST::CommentStatement& node) {
}

void CodeGeneratorVisitor::visit(AST::Story& node) {
    imageRuntimeRequired = false;
    collectImageUsage(&node);

    recordTypes.clear();
    recordTypeAliases.clear();
    std::vector<AST::RecordDeclaration*> records;
    collectRecords(&node, records);
    for (auto* record : records) {
        registerRecordType(*record);
    }

    oss << "#include <iostream>\n";
    oss << "#include <string>\n";
    oss << "#include <vector>\n";
    oss << "#include <cstdlib>\n";
    oss << "#include <ctime>\n";
    oss << "#include <unordered_map>\n";
    if (imageRuntimeRequired) {
        oss << "#include <algorithm>\n";
        oss << "#include <filesystem>\n";
        oss << "#include <fstream>\n";
    }
    oss << "\n";

    generateRandomizer();
    generateStoryStateHelpers();
    if (imageRuntimeRequired) {
        generateImageRuntime();
    }

    if (!records.empty()) {
        skipRecordDeclarations = false;
        for (auto* record : records) {
            record->accept(*this);
        }
    }

    collectionsUsed.clear();
    declaredCollections.clear();
    symbols.clear();
    symbolKinds.clear();
    initializedSymbols.clear();
    collectDeclarations(&node);
    collectCollections(&node);

    std::vector<AST::FunctionDeclaration*> functions;
    collectFunctions(&node, functions);
    for (auto* function : functions) {
        oss << "void " << function->name << "();\n";
    }
    if (!functions.empty()) {
        oss << "\n";
        skipFunctionDeclarations = false;
        for (auto* function : functions) {
            function->accept(*this);
        }
    }

    oss << "int main() {\n";
    indentLevel++;
    oss << indent() << "std::srand(static_cast<unsigned int>(std::time(nullptr)));\n\n";

    for (const auto& col : collectionsUsed) {
        if (declaredCollections.find(col) == declaredCollections.end()) {
            oss << indent() << "std::vector<std::string> " << col << " = {};\n";
        }
    }
    if (!collectionsUsed.empty()) {
        oss << "\n";
    }

    skipFunctionDeclarations = true;
    skipRecordDeclarations = true;
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
    }
    skipRecordDeclarations = false;
    skipFunctionDeclarations = false;

    oss << "\n" << indent() << "return 0;\n";
    indentLevel--;
    oss << "}\n";
}

void CodeGeneratorVisitor::visit(AST::VariableDeclaration& node) {
    std::string id = variableNameFor(node);
    if (node.isCollection()) {
        oss << indent() << "std::vector<std::string> " << id << " = {";
        for (size_t i = 0; i < node.values.size(); ++i) {
            if (i > 0) {
                oss << ", ";
            }
            oss << "\"" << escapeString(node.values[i]) << "\"";
        }
        oss << "};\n";
        return;
    }

    if (isNumberLiteral(node.value)) {
        std::string type = node.value.find('.') == std::string::npos ? "int" : "double";
        oss << indent() << type << " " << id << " = " << node.value << ";\n";
        initializedSymbols.insert(id);
        oss << indent() << "storyStates[\"" << escapeString(normalizeName(node.owner + " " + node.varName))
            << "\"] = std::to_string(" << id << ");\n";
    } else {
        oss << indent() << "std::string " << id << " = \"" << escapeString(node.value) << "\";\n";
        initializedSymbols.insert(id);
        if (node.varName == "state") {
            oss << indent() << "storyStates[\"" << escapeString(normalizeName(node.owner)) << "\"] = " << id << ";\n";
        } else {
            oss << indent() << "storyStates[\"" << escapeString(normalizeName(node.owner + " " + node.varName))
                << "\"] = " << id << ";\n";
        }
    }
}

void CodeGeneratorVisitor::visit(AST::VariableDeclarationBlock& node) {
    for (auto& decl : node.declarations) {
        decl->accept(*this);
    }
}

void CodeGeneratorVisitor::visit(AST::ArithmeticStatement& node) {
    std::string targetId = resolveName(node.target);
    if (targetId.empty()) {
        targetId = sanitizeIdentifier(node.target);
    }

    std::string cppOperator;
    if (node.operation == "add") {
        cppOperator = "+";
    } else if (node.operation == "subtract") {
        cppOperator = "-";
    } else if (node.operation == "multiply") {
        cppOperator = "*";
    } else if (node.operation == "divide") {
        cppOperator = "/";
    } else {
        throw std::runtime_error("Unsupported arithmetic operation: " + node.operation);
    }

    std::string expr = numericExpression(node.left) + " " + cppOperator + " " + numericExpression(node.right);
    bool targetsField = targetId.find('.') != std::string::npos;
    if (!targetsField && initializedSymbols.find(targetId) == initializedSymbols.end()) {
        oss << indent() << "double " << targetId << " = " << expr << ";\n";
        initializedSymbols.insert(targetId);
    } else {
        oss << indent() << targetId << " = " << expr << ";\n";
    }
    oss << indent() << "storyStates[\"" << escapeString(normalizeName(node.target))
        << "\"] = std::to_string(" << targetId << ");\n";
}

void CodeGeneratorVisitor::visit(AST::RecordDeclaration& node) {
    if (skipRecordDeclarations) {
        return;
    }

    registerRecordType(node);
    std::string typeId = sanitizeTypeName(node.name);
    oss << "struct " << typeId << " {\n";
    for (const auto& field : node.fields) {
        oss << "    " << cppTypeFor(field.second) << " " << sanitizeIdentifier(field.first)
            << " = " << cppDefaultValueFor(field.second) << ";\n";
    }
    oss << "};\n\n";
}

void CodeGeneratorVisitor::visit(AST::RecordInstanceDeclaration& node) {
    std::string id = variableNameFor(node);
    std::string typeId = cppTypeFor(node.typeName);
    if (initializedSymbols.find(id) == initializedSymbols.end()) {
        oss << indent() << typeId << " " << id << "{};\n";
        initializedSymbols.insert(id);
    }

    for (const auto& fieldValue : node.fieldValues) {
        std::string fieldName = fieldValue.first;
        std::string fieldType = fieldTypeFor(typeId, fieldName);
        oss << indent() << id << "." << sanitizeIdentifier(fieldName) << " = "
            << typedExpression(fieldValue.second, fieldType) << ";\n";
    }
}

void CodeGeneratorVisitor::visit(AST::ImageDeclaration& node) {
    std::string imageId = sanitizeIdentifier(node.name);
    oss << indent() << "OuatImage " << imageId << " = makeImage(static_cast<int>("
        << numericExpression(node.width) << "), static_cast<int>(" << numericExpression(node.height) << "));\n";
}

void CodeGeneratorVisitor::visit(AST::PixelWriteStatement& node) {
    oss << indent() << "paintPixel(" << sanitizeIdentifier(node.imageName)
        << ", static_cast<int>(" << numericExpression(node.x)
        << "), static_cast<int>(" << numericExpression(node.y)
        << "), " << numericExpression(node.red)
        << ", " << numericExpression(node.green)
        << ", " << numericExpression(node.blue)
        << ");\n";
}

void CodeGeneratorVisitor::visit(AST::ImageFillStatement& node) {
    oss << indent() << "fillImage(" << sanitizeIdentifier(node.imageName)
        << ", " << numericExpression(node.red)
        << ", " << numericExpression(node.green)
        << ", " << numericExpression(node.blue)
        << ");\n";
}

void CodeGeneratorVisitor::visit(AST::RectanglePaintStatement& node) {
    oss << indent() << "paintRectangle(" << sanitizeIdentifier(node.imageName)
        << ", static_cast<int>(" << numericExpression(node.left)
        << "), static_cast<int>(" << numericExpression(node.bottom)
        << "), static_cast<int>(" << numericExpression(node.right)
        << "), static_cast<int>(" << numericExpression(node.top)
        << "), " << numericExpression(node.red)
        << ", " << numericExpression(node.green)
        << ", " << numericExpression(node.blue)
        << ");\n";
}

void CodeGeneratorVisitor::visit(AST::ImageSaveStatement& node) {
    oss << indent() << "saveImageAsPpm(" << sanitizeIdentifier(node.imageName)
        << ", \"" << escapeString(node.outputPath) << "\");\n";
}

void CodeGeneratorVisitor::collectDeclarations(AST::Node* node) {
    if (auto variable = dynamic_cast<AST::VariableDeclaration*>(node)) {
        registerDeclaration(*variable);
    } else if (auto block = dynamic_cast<AST::VariableDeclarationBlock*>(node)) {
        for (auto& decl : block->declarations) {
            registerDeclaration(*decl);
        }
    } else if (auto record = dynamic_cast<AST::RecordDeclaration*>(node)) {
        registerRecordType(*record);
    } else if (auto recordInstance = dynamic_cast<AST::RecordInstanceDeclaration*>(node)) {
        registerRecordInstance(*recordInstance);
    } else if (auto arithmetic = dynamic_cast<AST::ArithmeticStatement*>(node)) {
        registerArithmeticTarget(*arithmetic);
    }

    if (auto story = dynamic_cast<AST::Story*>(node)) {
        for (auto& stmt : story->statements) {
            collectDeclarations(stmt.get());
        }
    } else if (auto cond = dynamic_cast<AST::ConditionalStatement*>(node)) {
        for (auto& stmt : cond->thenBranch) {
            collectDeclarations(stmt.get());
        }
        for (auto& stmt : cond->elseBranch) {
            collectDeclarations(stmt.get());
        }
    } else if (auto whileStmt = dynamic_cast<AST::WhileStatement*>(node)) {
        for (auto& stmt : whileStmt->body) {
            collectDeclarations(stmt.get());
        }
    } else if (auto forEach = dynamic_cast<AST::ForEachStatement*>(node)) {
        for (auto& stmt : forEach->body) {
            collectDeclarations(stmt.get());
        }
    } else if (auto forRange = dynamic_cast<AST::ForRangeStatement*>(node)) {
        symbols[normalizeName(forRange->iterator)] = sanitizeIdentifier(forRange->iterator);
        symbolKinds[sanitizeIdentifier(forRange->iterator)] = "number";
        for (auto& stmt : forRange->body) {
            collectDeclarations(stmt.get());
        }
    } else if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        for (auto& stmt : funcDecl->body) {
            collectDeclarations(stmt.get());
        }
    }
}

void CodeGeneratorVisitor::collectCollections(AST::Node* node) {
    if (auto fe = dynamic_cast<AST::ForEachStatement*>(node)) {
        std::string collectionName = resolveName(fe->collection);
        if (collectionName.empty()) {
            collectionName = sanitizeIdentifier(fe->collection);
        }
        collectionsUsed.insert(collectionName);
    }

    if (auto story = dynamic_cast<AST::Story*>(node)) {
        for (auto& stmt : story->statements) {
            collectCollections(stmt.get());
        }
    } else if (auto cond = dynamic_cast<AST::ConditionalStatement*>(node)) {
        for (auto& stmt : cond->thenBranch) {
            collectCollections(stmt.get());
        }
        for (auto& stmt : cond->elseBranch) {
            collectCollections(stmt.get());
        }
    } else if (auto whileStmt = dynamic_cast<AST::WhileStatement*>(node)) {
        for (auto& stmt : whileStmt->body) {
            collectCollections(stmt.get());
        }
    } else if (auto forEach = dynamic_cast<AST::ForEachStatement*>(node)) {
        for (auto& stmt : forEach->body) {
            collectCollections(stmt.get());
        }
    } else if (auto forRange = dynamic_cast<AST::ForRangeStatement*>(node)) {
        for (auto& stmt : forRange->body) {
            collectCollections(stmt.get());
        }
    } else if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        for (auto& stmt : funcDecl->body) {
            collectCollections(stmt.get());
        }
    }
}

void CodeGeneratorVisitor::collectRecords(AST::Node* node, std::vector<AST::RecordDeclaration*>& records) {
    if (auto record = dynamic_cast<AST::RecordDeclaration*>(node)) {
        records.push_back(record);
    }

    if (auto story = dynamic_cast<AST::Story*>(node)) {
        for (auto& stmt : story->statements) {
            collectRecords(stmt.get(), records);
        }
    } else if (auto cond = dynamic_cast<AST::ConditionalStatement*>(node)) {
        for (auto& stmt : cond->thenBranch) {
            collectRecords(stmt.get(), records);
        }
        for (auto& stmt : cond->elseBranch) {
            collectRecords(stmt.get(), records);
        }
    } else if (auto whileStmt = dynamic_cast<AST::WhileStatement*>(node)) {
        for (auto& stmt : whileStmt->body) {
            collectRecords(stmt.get(), records);
        }
    } else if (auto forEach = dynamic_cast<AST::ForEachStatement*>(node)) {
        for (auto& stmt : forEach->body) {
            collectRecords(stmt.get(), records);
        }
    } else if (auto forRange = dynamic_cast<AST::ForRangeStatement*>(node)) {
        for (auto& stmt : forRange->body) {
            collectRecords(stmt.get(), records);
        }
    } else if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        for (auto& stmt : funcDecl->body) {
            collectRecords(stmt.get(), records);
        }
    }
}

void CodeGeneratorVisitor::collectFunctions(AST::Node* node, std::vector<AST::FunctionDeclaration*>& functions) {
    if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        functions.push_back(funcDecl);
    }

    if (auto story = dynamic_cast<AST::Story*>(node)) {
        for (auto& stmt : story->statements) {
            collectFunctions(stmt.get(), functions);
        }
    } else if (auto cond = dynamic_cast<AST::ConditionalStatement*>(node)) {
        for (auto& stmt : cond->thenBranch) {
            collectFunctions(stmt.get(), functions);
        }
        for (auto& stmt : cond->elseBranch) {
            collectFunctions(stmt.get(), functions);
        }
    } else if (auto whileStmt = dynamic_cast<AST::WhileStatement*>(node)) {
        for (auto& stmt : whileStmt->body) {
            collectFunctions(stmt.get(), functions);
        }
    } else if (auto forEach = dynamic_cast<AST::ForEachStatement*>(node)) {
        for (auto& stmt : forEach->body) {
            collectFunctions(stmt.get(), functions);
        }
    } else if (auto forRange = dynamic_cast<AST::ForRangeStatement*>(node)) {
        for (auto& stmt : forRange->body) {
            collectFunctions(stmt.get(), functions);
        }
    } else if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        for (auto& stmt : funcDecl->body) {
            collectFunctions(stmt.get(), functions);
        }
    }
}

void CodeGeneratorVisitor::collectImageUsage(AST::Node* node) {
    if (dynamic_cast<AST::ImageDeclaration*>(node) ||
        dynamic_cast<AST::PixelWriteStatement*>(node) ||
        dynamic_cast<AST::ImageFillStatement*>(node) ||
        dynamic_cast<AST::RectanglePaintStatement*>(node) ||
        dynamic_cast<AST::ImageSaveStatement*>(node)) {
        imageRuntimeRequired = true;
    }

    if (auto story = dynamic_cast<AST::Story*>(node)) {
        for (auto& stmt : story->statements) {
            collectImageUsage(stmt.get());
        }
    } else if (auto cond = dynamic_cast<AST::ConditionalStatement*>(node)) {
        for (auto& stmt : cond->thenBranch) {
            collectImageUsage(stmt.get());
        }
        for (auto& stmt : cond->elseBranch) {
            collectImageUsage(stmt.get());
        }
    } else if (auto whileStmt = dynamic_cast<AST::WhileStatement*>(node)) {
        for (auto& stmt : whileStmt->body) {
            collectImageUsage(stmt.get());
        }
    } else if (auto forEach = dynamic_cast<AST::ForEachStatement*>(node)) {
        for (auto& stmt : forEach->body) {
            collectImageUsage(stmt.get());
        }
    } else if (auto forRange = dynamic_cast<AST::ForRangeStatement*>(node)) {
        for (auto& stmt : forRange->body) {
            collectImageUsage(stmt.get());
        }
    } else if (auto funcDecl = dynamic_cast<AST::FunctionDeclaration*>(node)) {
        for (auto& stmt : funcDecl->body) {
            collectImageUsage(stmt.get());
        }
    }
}

void CodeGeneratorVisitor::visit(AST::TellStatement& node) {
    oss << indent() << "std::cout << \"" << escapeString(node.message) << "\" << std::endl;\n";
}
