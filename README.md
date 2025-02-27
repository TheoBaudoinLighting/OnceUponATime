# OnceUponATime Compiler (`.ouat`)

## 📖 About the Project
**OnceUponATime (OUAT)** is a narrative programming language where you code by telling a story. The `ouat_compiler` translates `.ouat` scripts into C++ code, compiles it, and runs it.

---

## 🚀 Features
- **Natural Language Syntax** → Code by writing a story.
- **Automatic Class & Object Generation** → No need for manual struct/class declarations.
- **Real-Time C++ Code Generation** → Outputs `.cpp` files.
- **Compiles & Runs with `g++`** → Transforms `.ouat` files into executable programs.

---

## 📌 Example: Writing Code in `.ouat`
### **Example Script (`dragon_tale.ouat`)**
```txt
# Once Upon a Time (.ouat) script
# Author: Theo Baudoin
# Description: A simple story demonstrating the basic syntax and features
# of the .ouat language, including subjects, verbs, adjectives and objects.

# A tale of a knight and a dragon
# Demonstrates features: conditions and character states

Once upon a time.

A brave knight lived in a castle.
The knight saw a fearsome dragon.

The knight was uncertain if the dragon was friendly or hostile.

If the dragon was friendly then
    The knight and dragon became good friends.
    They shared stories and tea every afternoon.
Else if the dragon was not friendly then
    The knight challenged the dragon to a duel.
    The battle lasted three days and three nights.
Else
    The knight and dragon became enemies.
    They fought for control of the castle.

End.

The story ends.
```

### **Generated C++ Code (`output/generated.cpp / .exe`)**
```cpp
Parsed sentence structure:
Condition: the dragon was friendly
Then branch size: 2
Else branch size: 3

Entity states before code generation:
Entity States:
  dragon_is_hostile = false
  dragon_is_friendly = true
  dragon_is_or = true

Generated code:
----------------------------------------
#include <iostream>
#include <string>
#include <map>
#include <random>
#include <ctime>

// Function to generate a random boolean
bool getRandomBool() {
    return std::rand() % 2 == 0;
}

int main() {
    // Initialize the random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    // Entity state declarations
    bool dragon_is_hostile = false;
    bool dragon_is_friendly = true;
    bool dragon_is_or = true;

    std::cout << "A brave knight lived in a castle" << std::endl;
    std::cout << "The knight saw a fearsome dragon" << std::endl;
    // Randomly determine the state of dragon
    bool randomChoice = getRandomBool();
    dragon_is_friendly = randomChoice;
    dragon_is_hostile = !randomChoice;
    std::cout << "The dragon was " << (randomChoice ? "friendly" : "hostile") << "." << std::endl;
    if (dragon_is_friendly) {
        std::cout << "The knight and dragon became good friends" << std::endl;
        std::cout << "They shared stories and tea every afternoon" << std::endl;
    } else {
        if (!dragon_is_friendly) {
            std::cout << "The knight challenged the dragon to a duel" << std::endl;
            std::cout << "The battle lasted three days and three nights" << std::endl;
        }
        std::cout << "The knight and dragon became enemies" << std::endl;
        std::cout << "They fought for control of the castle" << std::endl;
    }
    return 0;
}
```
## And return 
```
A brave knight lived in a castle
The knight saw a fearsome dragon
The dragon was friendly.
The knight and dragon became good friends
They shared stories and tea every afternoon
```
---

## 🛠️ Installation
### **Prerequisites**
- `g++` (GCC Compiler)
- C++17 or later

### **Cloning the Repository**
```sh
git clone https://github.com/yourusername/OnceUponATime.git
cd OnceUponATime
```

### **Building the Compiler**
```sh
mkdir build && cd build
cmake ..
make
```

---

## 🎯 Usage
### **Compiling a `.ouat` File**
```sh
./bin/ouat_compiler examples/example.ouat -o output/scene.cpp
```

### **Running the Generated Code**
```sh
g++ output/scene.cpp -o output/scene
./output/scene
```

---

## 🧪 Running Tests
```sh
make test
```

---

## 🛠 Development Roadmap

### **Phase 1: Language Specification & Design**
- ✅ **Create Lexer** → Lexical Analyzer.
- ✅ **Define Syntax & Grammar** → Establish rules for `.ouat` syntax.
- ✅ **Create Keyword Dictionary** → Define supported verbs, nouns, and adjectives.
- ✅ **Write Example `.ouat` Scripts** → Showcase valid scripts and edge cases.
- ✅ **Document Language Rules** → Create `docs/specs_ouat.md`.

### **Phase 2: Lexer Implementation**
- ✅ **Tokenization of `.ouat` Scripts** → Convert words into structured tokens.
- ✅ **Regular Expressions for Syntax Matching** → Identify key tokens (objects, actions, properties).
- ❌ **Basic Error Handling in Tokenizer** → Detect invalid syntax early.
- ❌ **Unit Tests for Tokenization** → Validate Lexer correctness.

### **Phase 3: Parser & AST Construction**
- ❌ **Define Abstract Syntax Tree (AST) Structure** → Represent relationships between elements.
- ✅ **Implement Recursive Descent Parser** → Convert tokens into structured AST.
- ❌ **Error Recovery in Parsing** → Handle incorrect or unexpected syntax.
- ❌ **Unit Tests for Parser** → Ensure correct AST generation.

### **Phase 4: Code Generation**
- ❌ **Map AST to C++ Code** → Convert high-level `.ouat` structures to compilable C++.
- ❌ **Generate Classes & Functions Dynamically** → Infer class definitions from script.
- ❌ **Optimize Generated Code** → Ensure efficiency in execution.
- ❌ **Unit Tests for Code Generation** → Validate output correctness.

### **Phase 5: Compiler Execution**
- ❌ **Compile Generated C++ with `g++`** → Automate compilation step.
- ❌ **Run Executable & Capture Output** → Ensure expected program behavior.
- ❌ **Error Logging & Debug Messages** → Provide feedback on failures.
- ❌ **Automate Compilation Process** → Single-command execution.

### **Phase 6: Advanced Features**
- ❌ **Implement Conditional Logic (`if`, `else`)** → Enable branching behavior.
- ❌ **Looping Constructs (`while`, `for`)** → Support iteration in `.ouat`.
- ❌ **Function Definition & Invocation** → Allow reusable story segments.
- ❌ **Memory Management & Cleanup** → Prevent resource leaks in runtime.

### **Phase 7: Tooling & Debugging**
- ❌ **Interactive Debugger** → Step through execution & inspect state.
- ❌ **Editor Support with Syntax Highlighting** → Improve developer experience.
- ❌ **Logging & Profiling Tools** → Monitor execution performance.
- ❌ **Extensible Plugin System** → Enable future feature expansions.

---

## 🤝 Contributing
Pull requests are welcome! Please check the `docs/specs_ouat.md` before contributing.

---

## 📄 License
MIT License - See `LICENSE` for details.

