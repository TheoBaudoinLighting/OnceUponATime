# OnceUponATime Compiler (.ouat) – Organic Narrative Programming

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Contributors](https://img.shields.io/github/contributors/TheoBaudoinLighting/OnceUponATime)](https://github.com/TheoBaudoinLighting/OnceUponATime/graphs/contributors)
[![GitHub Stars](https://img.shields.io/github/stars/TheoBaudoinLighting/OnceUponATime?style=social)](https://github.com/TheoBaudoinLighting/OnceUponATime/stargazers)

**OnceUponATime (OUAT)** is an innovative and immersive programming language that enables writing code through natural, poetic, and engaging storytelling. Each .ouat script is transformed into modern C++17 code that can be compiled with a standard C++ compiler.

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
- **User-Defined Records:** Define structured data such as vectors, rays, domain objects, or app entities without adding domain-specific syntax.
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

**Variables, Collections, and Conditions:**
- Declaration and initialization: `has a`, `has`, direct assignment: `is`
- Collections: `The hero has companions of ["Alice", "Bob"].`, `Squad has members ["Alice", "Bob"].`
- Conditions: `is`, `is not`, `equals`, `is greater than`, `is less than`, `is at least`, `is at most`
- Numbers: integers and decimals such as `10`, `0.5`, `3.14`
- Arithmetic mutation: `Magic subtract 1 equals magic.`
- Numeric range loops: `For each y from 0 to height do ... endfor.`

**Records and Structured Data:**
- Type declaration: `Define the record Vec3 with x number and y number and z number.`
- Instance declaration: `The color is a Vec3 with x 1 and y 0.5 and z 0.25.`
- Field access in expressions: `Paint canvas at 0 0 with color x, color y, color z.`

**Interaction and Randomness:**
- User interaction: `choose`, `input`
- Randomization: `random`, `uncertain`, `fate leans`

**Display and Immersive Narration:**
- Simple display: `display`
- Immersive narration: `narrate`
- Alternative display: `tell`

**Image Output:**
- Create an RGB image: `Create image canvas with width image width and height image height.`
- Paint a pixel: `Paint canvas at x y with red green blue.`
- Fill an image with a named color: `Fill image canvas with back wall.`
- Paint a rectangle: `Paint rectangle on canvas from 0 0 to 32 160 with left wall.`
- Save as PPM: `Save image canvas to "output/image.ppm".`

**Narrative Comments:**
- Must begin with `Remark:`, `Note:`, or `Comment:` and end with a period.

---

## Detailed Installation

### Prerequisites
- Visual Studio 2022 / MSBuild for the included solution, or another C++17 compiler for generated C++ files
- Version control: Git

### Complete Installation

Clone and build the Visual Studio solution:
```sh
git clone https://github.com/yourusername/OnceUponATime.git
cd OnceUponATime
MSBuild OnceUponATime.sln /p:Configuration=Debug /p:Platform=x64
```

---

## User Guide

**Compiling a .ouat script:**
```sh
build/Debug/OnceUponATime.exe
```

**Compiling & running the generated program:**
```sh
cl /EHsc /std:c++17 OnceUponATime/output/generated.cpp
generated.exe
```

---

## Extended Example of a .ouat Script

```txt
Once upon a time.

Remark: The adventure begins in the mysterious kingdom of Æther.
The hero has a strength of 10 and magic is 5.
The hero has companions of ["Alice", "Bob", "Charlie"].
The dragon is awake.
Define the record Vec3 with x number and y number and z number.
The ember color is a Vec3 with x 1 and y 0.25 and z 0.1.
Magic subtract 1 equals magic.

If the dragon is awake then
    display "The dragon roars with mystical fury".
    Define the function invokeShield as
        display "A luminous shield protects the hero from imminent danger".
        return.
    endfunction.
    call invokeShield.
else if the dragon is friendly then
    display "A surprising alliance forms between the hero and the dragon".
else
    display "Conflict erupts violently, and dark shadows descend".
endif.

For each companion in hero companions do
    tell "A brave companion joins the legendary adventure".
endfor.

For each row from 0 to 3 do
    tell "A row of the image is prepared".
endfor.

The hero choose "Which path should be followed now?".
Random fate leans towards courage or caution.
Comment: End of the script.

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
- ✅ **Compile Generated C++ with a C++17 compiler** → Automate compilation step.
- ✅ **Run Executable & Capture Output** → Ensure expected program behavior.
- ✅ **Error Logging & Debug Messages** → Provide feedback on failures.
- ✅ **Automate Compilation Process** → Single-command execution.

### **Phase 6: Advanced Features**
- ✅ **Implement Conditional Logic (`if`, `else`)** → Enable branching behavior.
- ✅ **Looping Constructs (`while`, `for`)** → Support iteration in `.ouat`.
- ✅ **Function Definition & Invocation** → Allow reusable story segments.
- ✅ **Numeric Literals & Arithmetic Mutation** → Support integer/decimal values and narrative arithmetic forms.
- ✅ **Numeric Range Loops** → Support `For each i from start to end do`.
- ✅ **General Image Output** → Create images, paint pixels, and save PPM files.
- ✅ **Narrative Raster Actions** → Fill images and paint named rectangles from general color records.
- ✅ **Memory-Safe AST Ownership** → Use RAII and `std::unique_ptr` for compiler-owned nodes.

### **Phase 7: General Compute & Rendering**
- ❌ **Function Parameters & Return Values** → Required for reusable math and rendering functions.
- ✅ **Records / Structured Types** → Support generic user-defined data such as `Vec3`, `Ray`, app entities, and scene records.
- ❌ **Vector and Math Primitives** → Required for clean vector algebra without C++ escape hatches.
- ❌ **Cornell Box Path Tracer Example** → Must be written from general OUAT features, not a path-tracer-specific command.

### **Phase 8: Tooling & Debugging**
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

