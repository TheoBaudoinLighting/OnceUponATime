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
### **Example Script (`example.ouat`)**
```txt
Once upon a time.
Inside a house of 10 meters, there was a golden chalice.
The golden chalice shone brightly.
The story ends.
```

### **Generated C++ Code (`output/scene.cpp`)**
```cpp
#include <iostream>
#include <vector>

class House {
public:
    float size;
    std::vector<std::string> objects_inside;
    House(float s) : size(s) {}
    void addObject(std::string obj) { objects_inside.push_back(obj); }
};

class Chalice {
public:
    std::string color;
    Chalice(std::string c) : color(c) {}
    void shine() { std::cout << "The " << color << " chalice shines brightly!" << std::endl; }
};

int main() {
    House house(10);
    Chalice chalice("golden");
    house.addObject("golden chalice");
    chalice.shine();
    return 0;
}
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
- ❌ **Define Syntax & Grammar** → Establish rules for `.ouat` syntax.
- ❌ **Create Keyword Dictionary** → Define supported verbs, nouns, and adjectives.
- ❌ **Write Example `.ouat` Scripts** → Showcase valid scripts and edge cases.
- ❌ **Document Language Rules** → Create `docs/specs_ouat.md`.

### **Phase 2: Lexer Implementation**
- ❌ **Tokenization of `.ouat` Scripts** → Convert words into structured tokens.
- ❌ **Regular Expressions for Syntax Matching** → Identify key tokens (objects, actions, properties).
- ❌ **Basic Error Handling in Tokenizer** → Detect invalid syntax early.
- ❌ **Unit Tests for Tokenization** → Validate Lexer correctness.

### **Phase 3: Parser & AST Construction**
- ❌ **Define Abstract Syntax Tree (AST) Structure** → Represent relationships between elements.
- ❌ **Implement Recursive Descent Parser** → Convert tokens into structured AST.
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

