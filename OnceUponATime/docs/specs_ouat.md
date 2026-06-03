# OnceUponATime Language Specification

## 📖 Introduction
The **OnceUponATime (`.ouat`)** language is an innovative programming language designed for writing code through structured, narrative storytelling. This document details the syntax, grammar, and comprehensive rules required to craft valid `.ouat` programs.

---

## 📌 Program Structure

Every `.ouat` program must strictly follow this narrative structure:
- **Prologue:** The program begins with the exact phrase `Once upon a time.`
- **Narrative Body:** Composed of structured sentences that represent program logic, each ending with a period.
- **Epilogue:** The program concludes explicitly with `The story ends.`

### **Example of a Valid .ouat Program**
```txt
Once upon a time.
Inside a castle of 100 meters, there was an ancient sword.
The ancient sword shone brightly under the moonlight.
The story ends.
```

---

## 📜 Syntax Rules

### **1️⃣ Header and Epilogue Rules**
- **Header (Prologue):** Every script must start with `Once upon a time.`
- **Footer (Epilogue):** Every script must end with `The story ends.`

### **2️⃣ Sentence Structure**
Each sentence must follow a clear subject-verb-object order to ensure precise parsing.

✅ **Correct Examples:**
```txt
The knight rode the horse.
The dragon breathed fire.
```
❌ **Incorrect Examples:**
```txt
Horse rode knight the.
Fire breathed dragon.
```

### **3️⃣ Object Declaration and Initialization**
Objects and characters must be declared explicitly:
```txt
The hero has strength of 10.
The hero has companions of ["Alice", "Bob", "Charlie"].
Squad has members ["Alice", "Bob", "Charlie"].
```

### **4️⃣ Properties and States**
Properties and states of objects must be clearly expressed:
```txt
The gate is locked.
The dragon is awake.
```

### **5️⃣ Control Flow**
Control structures manage program logic:
```txt
If the door is locked then
    display "The hero cannot pass".
else
    display "The hero opens the door".
endif.
```

### **5️⃣ Loops and Iteration**
Loops follow a structured syntax:
```txt
While the dragon is awake.
    display "The dragon breathes fire".
endwhile.

For each coin in the chest do
    display "The hero takes a coin".
endfor.
```

### **6️⃣ Functions and Reusable Blocks**
Functions allow reusable narrative blocks:
```txt
Define the function healHero as
    display "The hero regains strength".
    return.
endfunction.

call healHero.
```

### **6️⃣ Records and Structured Data**
Records define reusable data shapes. They are general-purpose: the same syntax can describe vectors, rays, invoices, users, scene objects, or any other structured value.
```txt
Define the record Vec3 with x number and y number and z number.
The color is a Vec3 with x 1 and y 0.5 and z 0.25.
```

Fields can be used by writing the instance name followed by the field name:
```txt
Paint canvas at 0 0 with color x, color y, color z.
```

### **6️⃣ Interaction and Randomization**
Interactions with the user and randomness are clearly defined:
```txt
The hero choose "Which path should be taken?".
Random fate leans towards bravery or caution.
```

### **7️⃣ Comments**
Comments provide context without affecting the execution:
- Begin with `Remark:`, `Note:`, or `Comment:`, and end with a period.

```txt
Remark: This introduces the main character's motives.
Note: Initialize health points here.
Comment: Descriptive section begins.
```

---

## 📌 Complete List of Keywords

**Structural Keywords:**
- Prologue: `Once upon a time.`
- Epilogue: `The story ends.`

**Control Flow Keywords:**
- Conditions: `If`, `then`, `else if`, `else`, `endif.`
- Loops: `While`, `endwhile.`, `For each`, `do`, `endfor.`

**Function Keywords:**
- Declaration: `Define the function [name] as`, ends with `endfunction.`
- Invocation: `call`
- Return: `return`

**Declaration and Assignment:**
- Declaration: `has a`, `has`
- Assignment: `is`
- Numeric literals: integers and decimals such as `10`, `0.5`, `3.14`

**Records and Structured Data:**
- Type declaration: `Define the record Vec3 with x number and y number and z number.`
- Instance declaration: `The color is a Vec3 with x 1 and y 0.5 and z 0.25.`
- Field access: `color x`, `color y`, `color z`

**Condition Operators:**
- State equality: `is`, `is not`, `equals`
- Numeric comparison: `is greater than`, `is less than`, `is at least`, `is at most`

**Arithmetic Mutation:**
- `A add B equals C.`
- `A subtract B equals C.`
- `A multiply B equals C.`
- `A divide B equals C.`

These forms assign the result to `C`. Example:
```txt
Magic subtract 1 equals magic.
```

**Numeric Range Loops:**
```txt
For each y from 0 to height do
    tell "A row is visited".
endfor.
```

**Image Output:**
OUAT includes general image primitives for any program that needs to produce pixels:
```txt
The image has width of 200 and height is 100.
Create image canvas with width image width and height image height.
Fill image canvas with back wall.
Paint canvas at 0 0 with 1 0 0.
Paint rectangle on canvas from 0 0 to 32 100 with left wall.
Save image canvas to "output/image.ppm".
```

Color values are normalized from `0.0` to `1.0`, and images are saved as PPM files.
Named colors are regular records with `red`, `green`, and `blue` fields.

**Interaction and Randomness:**
- User Interaction: `choose`, `input`
- Randomization: `random`, `uncertain`, `fate leans`

**Narrative Display:**
- Display: `display`
- Immersive Narration: `narrate`
- Alternative: `tell`

**Narrative Comments:**
- Comments: `Remark:`, `Note:`, `Comment:`

**Adjectives and Descriptive Attributes:**
- `ancient`, `magical`, `golden`, `fierce`, `locked`, `bright`, `heroic`

---

## 🛠 Parsing Examples

Parsing clearly structured sentences:

**Example:**
```txt
The wizard casts a spell.
```

Parsed as:
```
Subject: The wizard
Verb: casts
Object: a spell
```

---

## 📌 Error Handling

Common errors and corrections:

- **Missing punctuation**:
❌ Incorrect:
```txt
The wizard casts a spell
```
✅ Correct:
```txt
The wizard casts a spell.
```

- **Improper structure**:
❌ Incorrect:
```txt
Spell casts wizard the.
```
✅ Correct:
```txt
The wizard casts a spell.
```

- **Missing program markers**:
❌ Incorrect:
```txt
The dragon roared.
```
✅ Correct:
```txt
Once upon a time.
The dragon roared.
The story ends.
```

---

## 🔍 Future Extensions

- Support for dialogues (`"Greetings," said the knight.`)
- Function parameters and return values
- Vector and math standard-library primitives
- Full path-tracing examples built from general language features
- Advanced conditional logic (`If the dragon was asleep, the hero approached silently.`)
- Event-driven programming features

This specification outlines the formal grammar and rules for writing structured, narrative-driven `.ouat` programs, serving as the authoritative reference as the language evolves.

---

## 📄 License

MIT License – See `LICENSE` for full details.

