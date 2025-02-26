# OnceUponATime Language Specification

## 📖 Introduction
The **OnceUponATime (`.ouat`)** language is a narrative-driven programming language where code is written as a structured story. This document defines the syntax, grammar, and rules for writing `.ouat` programs.

---

## 📌 Program Structure
Every `.ouat` program follows a structured storytelling format:
- It **must** begin with `Once upon a time.`
- It **must** end with `The story ends.`
- Sentences describe objects, actions, and relationships.

### **Example Program**
```txt
Once upon a time.
Inside a house of 10 meters, there was a golden chalice.
The golden chalice shone brightly.
The story ends.
```

---

## 📜 Syntax Rules
### **1️⃣ Header Rule**
A valid `.ouat` program **must** begin with:
```txt
Once upon a time.
```
This acts as the entry point of the script.

### **2️⃣ Sentence Structure**
Each sentence must follow a **structured format**:
```
[Subject] [Verb] [Object].
```
✅ **Valid Examples:**
```txt
Arthur walked into the castle.
The dragon breathed fire.
```
❌ **Invalid Examples:**
```txt
Castle into walked Arthur.
Dragon fire the breathed.
```

### **3️⃣ Object Declaration**
Objects in the world are introduced using `There was` or `Inside ... there was`.
```txt
There was a sword.
Inside the castle, there was a throne.
```

### **4️⃣ Actions & Verbs**
Actions are performed by characters or objects using a **verb**.
```txt
The knight swung his sword.
The torch flickered in the wind.
```

### **5️⃣ Attributes & Properties**
Objects and characters can have attributes like color, size, or state.
```txt
The door was wooden.
The dragon was fierce and red.
```

### **6️⃣ Ending Rule**
A valid `.ouat` program **must** end with:
```txt
The story ends.
```

---

## 📌 Keywords & Tokens
### **1️⃣ Keywords** (Reserved Words)
- `Once upon a time` → **Start of program**
- `The story ends` → **End of program**
- `Inside`, `There was`, `The` → **Object declarations**

### **2️⃣ Verbs** (Actions)
The following are valid action words:
- `walked`, `ran`, `jumped`
- `shone`, `flickered`, `breathed`
- `held`, `carried`, `threw`

### **3️⃣ Adjectives** (Attributes)
- `golden`, `wooden`, `red`, `old`, `shiny`
- `big`, `small`, `fierce`, `ancient`

---

## 🛠 Example Parsing
A sentence like:
```txt
The knight swung his sword.
```
Would be parsed into:
```
Subject: The knight
Verb: swung
Object: his sword
```

---

## 📌 Error Handling
### **1️⃣ Missing Punctuation**
❌ Invalid:
```txt
The knight swung his sword
```
✅ Correct:
```txt
The knight swung his sword.
```

### **2️⃣ Incorrect Ordering**
❌ Invalid:
```txt
Sword his swung knight the.
```
✅ Correct:
```txt
The knight swung his sword.
```

### **3️⃣ Missing Story Markers**
❌ Invalid:
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
- ✅ Implementing dialogue (`"Hello," said the knight.`)
- ✅ Adding numeric values (`The tower was 50 meters tall.`)
- ✅ Supporting conditional events (`If the door was locked, the knight knocked.`)

This document serves as the **formal specification** for the `.ouat` language. More features will be added as the language evolves.

---

## 📄 License
MIT License - See `LICENSE` for details.

