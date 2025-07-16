# OnceUponATime Compiler (.ouat) – Organic Narrative Programming

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Contributors](https://img.shields.io/github/contributors/TheoBaudoinLighting/OnceUponATime)](https://github.com/TheoBaudoinLighting/OnceUponATime/graphs/contributors)
[![GitHub Stars](https://img.shields.io/github/stars/TheoBaudoinLighting/OnceUponATime?style=social)](https://github.com/TheoBaudoinLighting/OnceUponATime/stargazers)

**OnceUponATime (OUAT)** is an innovative and immersive programming language that enables writing code through natural, poetic, and engaging storytelling. Each .ouat script is automatically transformed into modern C++ code, ready for direct compilation and execution using the g++ compiler.

---

## About the Project

OUAT revolutionizes programming by fully integrating narrative artistry with technical software development. By structuring algorithmic logic as stories, OUAT allows developers, writers, and artists to create programs that literally tell stories. Each element of the language is meticulously designed to provide an intuitive yet powerful experience, making coding accessible while maintaining impeccable technical precision.

The project aims to bridge the gap between literary arts and computer science, promoting mutual understanding and fostering multidisciplinary creativity.

---

## Key Features

- **Natural and Organic Syntax:** Write your programs as authentic and vibrant narratives.
- **Automatic Compilation to C++:** Every OUAT script is instantly converted into clear, efficient, and compilable C++ code (compatible with C++17 and later).
- **Conditional Instructions and Loops:** Intuitively handle conditions (`If ... then`, `else if`, `else`, `endif.`) and loops (`While`, `endwhile.`, `For each`, `do`, `endfor.`).
- **Reusable Functions and Sub-narratives:** Clearly define and easily invoke narrative instruction blocks as functions.
- **Integrated Organic Comments:** Naturally annotate your scripts with comments starting with `Remark:`, `Note:`, or `Comment:`, without affecting execution.
- **User Interaction and Random Elements:** Easily incorporate interactive choices and random elements to enhance narrative experience.

---

## Complete Syntax and Keywords

### Structure of a .ouat Script

- **Mandatory Prologue:** `Once upon a time.`
- **Narrative Body:** Each sentence corresponds to an instruction, declaration, condition, or loop, and must end with a period.
- **Mandatory Epilogue:** `The story ends.`

### Main Keywords

**Flow Control:**
- Conditions: `If`, `then`, `else if`, `else`, `endif.`
- Loops: `While`, `endwhile.`, `For each`, `do`, `endfor.`

**Functions:**
- Declaration: `Define the function [name] as`, closed with `endfunction.`
- Invocation: `call [name]`
- Function return: `return`

**Variables and Operations:**
- Declaration and initialization: `has a`, `has`, direct assignment: `is`
- Arithmetic operations: `add`, `subtract`, `multiply`, `divide`, equality check: `equals`

**Interaction and Randomness:**
- User interaction: `choose`, `input`
- Randomization: `random`, `uncertain`, `fate leans`

**Display and Immersive Narration:**
- Simple display: `display`
- Immersive narration: `narrate`
- Alternative display: `tell`

**Narrative Comments:**
- Must begin with `Remark:`, `Note:`, or `Comment:` and end with a period.

---

## Detailed Installation

### Prerequisites
- g++ compiler (C++17 or later)
- Build tool: CMake
- Version control: Git

### Complete Installation

Clone and compile the project easily:
```sh
git clone https://github.com/yourusername/OnceUponATime.git
cd OnceUponATime
mkdir build && cd build
cmake ..
make
```

---

## User Guide

**Compiling a .ouat script:**
```sh
./bin/ouat_compiler examples/my_story.ouat -o output/scene.cpp
```

**Compiling & running the generated program:**
```sh
g++ output/scene.cpp -o output/scene
./output/scene
```

---

## Extended Example of a .ouat Script

```txt
Once upon a time.

Remark: The adventure begins in the mysterious kingdom of Æther.
The hero has a strength of 10 and magic is 5.
The sky was vibrant with strange colors until the sudden appearance of a dragon.
Fate was uncertain, and the hero's heart beat with heroic intensity.

If the dragon awakens then
    display "The dragon roars with mystical fury."
    Define the function invokeShield as
        display "A luminous shield protects the hero from imminent danger."
        return
    endfunction.
    call invokeShield.
else if the dragon is friendly then
    display "A surprising alliance forms between the hero and the dragon, promising a new future."
else
    display "Conflict erupts violently, and dark shadows descend."
    While magic is greater than 0
        narrate "Sparks dance in the deep darkness."
        Magic subtract 1 equals magic.
    endwhile.
endif.

For each companion in the squad do
    tell "A brave companion joins the legendary adventure."
endfor.

The hero choose "Which path should be followed now?".
Random fate leans towards courage or caution.
Commentary: End of the script.

The story ends.
```

---

## Development Roadmap

### **Phase 1: Language Specification & Design**
- ✅ **Create Lexer** → Lexical Analyzer.
- ✅ **Define Syntax & Grammar** → Establish rules for `.ouat` syntax.
- ✅ **Create Keyword Dictionary** → Define supported verbs, nouns, and adjectives.
- ✅ **Write Example `.ouat` Scripts** → Showcase valid scripts and edge cases.
- ✅ **Document Language Rules** → Create `docs/specs_ouat.md`.

### **Phase 2: Lexer Implementation**
- ✅ **Tokenization of `.ouat` Scripts** → Convert words into structured tokens.
- ✅ **Regular Expressions for Syntax Matching** → Identify key tokens (objects, actions, properties).
- ✅ **Basic Error Handling in Tokenizer** → Detect invalid syntax early.
- ✅ **Unit Tests for Tokenization** → Validate Lexer correctness.

### **Phase 3: Parser & AST Construction**
- ✅ **Define Abstract Syntax Tree (AST) Structure** → Represent relationships between elements.
- ✅ **Implement Recursive Descent Parser** → Convert tokens into structured AST.
- ❌ **Error Recovery in Parsing** → Handle incorrect or unexpected syntax.
- ✅ **Unit Tests for Parser** → Ensure correct AST generation.

### **Phase 4: Code Generation**
- ✅ **Map AST to C++ Code** → Convert high-level `.ouat` structures to compilable C++.
- ✅ **Generate Classes & Functions Dynamically** → Infer class definitions from script.
- ✅ **Optimize Generated Code** → Ensure efficiency in execution.
- ✅ **Unit Tests for Code Generation** → Validate output correctness.

### **Phase 5: Compiler Execution**
- ✅ **Compile Generated C++ with `g++`** → Automate compilation step.
- ✅ **Run Executable & Capture Output** → Ensure expected program behavior.
- ✅ **Error Logging & Debug Messages** → Provide feedback on failures.
- ✅ **Automate Compilation Process** → Single-command execution.

### **Phase 6: Advanced Features**
- ✅ **Implement Conditional Logic (`if`, `else`)** → Enable branching behavior.
- ✅ **Looping Constructs (`while`, `for`)** → Support iteration in `.ouat`.
- ✅ **Function Definition & Invocation** → Allow reusable story segments.
- ❌ **Memory Management & Cleanup** → Prevent resource leaks in runtime.

### **Phase 7: Tooling & Debugging**
- ❌ **Interactive Debugger** → Step through execution & inspect state.
- ❌ **Editor Support with Syntax Highlighting** → Improve developer experience.
- ❌ **Logging & Profiling Tools** → Monitor execution performance.
- ❌ **Extensible Plugin System** → Enable future feature expansions.

---

## Contributing
Pull requests are welcome! Please check the `docs/specs_ouat.md` before contributing.

---

## 📄 License
MIT License - See `LICENSE` for details.

