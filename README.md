# C2Go: A C-to-Go Source-to-Source Compiler

**C2Go** is a source-to-source compiler (transpiler) written in **C** that translates a defined subset of **C** source code into idiomatic, valid **Go** source code.

The primary objective is to explore low-level systems programming, compiler theory, language semantics, and abstract syntax tree manipulation without relying on external compiler frameworks (like LLVM or Yacc/Bison).

---

## 🚀 Core Concept

```text
C Source Code (.c)
       │
       ▼
┌─────────────────────────────────┐
│   C2Go Compiler (Written in C)  │
└─────────────────────────────────┘
       │
       ▼
Generated Go Source (.go)
       │
       ▼
┌─────────────────────────────────┐
│      Official Go Toolchain      │
└─────────────────────────────────┘
       │
       ▼
Native Executable (.exe)
```

- **Implementation Language:** C (C99 / C11)
- **Source Language:** Defined, documented subset of C
- **Target Language:** Go
- **Final Binary:** Compiled with the official Go toolchain (`go build`)

> **Note:** The compiler itself is strictly implemented in C, not Go.

---

## 🎯 Project Goals

### Primary Goals
- **Self-contained compiler in C:** Build a recursive descent parser, custom lexer, and AST representation from scratch in C.
- **Accurate Translation:** Emit valid Go code that accurately preserves the observable semantics and execution behavior of the input C code.
- **Deep Understanding:** Learn compiler internals, memory arenas, symbol tables, and type-checking mechanics directly.
- **Well-defined Subset:** Clearly define and enforce the supported C feature set, emitting meaningful error diagnostics on unsupported syntax.
- **Validation:** Verify translated programs against reference C compilers (GCC / Clang).

### Future Goals
- Implement a platform-independent Intermediate Representation (IR).
- Add an alternative backend: compile the IR into a custom bytecode format.
- Build a custom virtual machine (VM) in C to execute the bytecode.
- Set up automated cross-platform regression test suites.

---

## 🏗️ Compiler Architecture & Pipeline

```text
C Source Text
     │
     ▼
[ Lexer ] ────────────► Token Stream (with source locations)
     │
     ▼
[ Parser ] ───────────► Abstract Syntax Tree (AST)
     │
     ▼
[ Semantic Analysis ] ─► Type checking, scopes, symbol tables
     │
     ▼
[ IR Generation ] ────► Intermediate Representation (optional/future)
     │
     ▼
[ Code Generator ] ───► Syntactically valid, formatted Go code
     │
     ▼
Generated Go Source (.go)
```

### Component Breakdown
- **Arena Allocator (`arena.c` / `arena.h`):** Efficient bump allocation for compiler memory lifetimes (tokens, AST nodes).
- **Lexer (`lexer.c` / `lexer.h`):** Scans source bytes and outputs token streams with line and column tracking for diagnostics.
- **Parser (`parser.c` / `parser.h`):** Constructs an AST with proper precedence parsing for expressions and statements.
- **AST (`ast.h`):** Explicit tree representation for declarations, functions, statements, and expressions.
- **Semantic Analyzer (`sema.c` / `sema.h`):** Symbol table resolution, scope verification, and type checking.
- **Go Backend (`codegen.c` / `codegen.h`):** Serializes AST nodes to clean Go code, managing C vs. Go semantic differences (such as `main` exit codes and signed integer overflows).
- **CLI Driver (`main.c`):** Command-line interface accepting input paths and output targets.

---

## 📂 Project Structure

```text
c2go/
├── compiler/           # Compiler implementation (C sources)
│   ├── main.c          # Compiler entry point & CLI
│   ├── arena.c         # Memory arena implementation
│   ├── lexer.c         # Lexical analyzer (planned)
│   ├── parser.c        # Syntax analyzer & AST builder (planned)
│   ├── sema.c          # Semantic analyzer & type checker (planned)
│   └── codegen.c       # Go code generator (planned)
├── include/            # Header files
│   ├── arena.h         # Arena allocator interfaces
│   ├── lexer.h         # Token definitions & lexer API
│   ├── parser.h        # Parser API (planned)
│   ├── ast.h           # AST structure definitions (planned)
│   └── codegen.h       # Code generator API (planned)
├── examples/           # C input programs to translate
│   └── hello.c         # Target test programs
├── tests/              # Test suites and test runners
├── output/             # Generated output files (Go sources, binaries)
└── README.md
```

---

## 🔍 Language Scope & Roadmap

We prioritize **correctness over breadth**. Unsupported constructs will produce explicit compiler diagnostics rather than silently producing broken Go.

### Initial Supported C Subset
- [x] Basic function declarations and definitions (`main(void)`)
- [x] Integer return statements (`return 42;`)
- [ ] Fixed-width and standard integer types (`int`, `int32_t`)
- [ ] Binary arithmetic expressions (`+`, `-`, `*`, `/`) with precedence
- [ ] Local variable declarations and assignments
- [ ] Boolean logic and control flow (`if` / `else`, `while`, `for`)
- [ ] User-defined functions, argument passing, and call expressions

### Explicitly Deferred Features
- Pointer arithmetic and raw pointers
- Manual memory management (`malloc`, `free`)
- Structs and unions
- Preprocessor macros (beyond basic headers)
- Variadic functions and function pointers

---

## 📍 Implementation Roadmap

- **Milestone 1: Minimal Proof of Concept**
  - Accept `int main(void) { return 42; }` and generate a working Go program that compiles and exits cleanly.
- **Milestone 2: Lexer & Tokenizer**
  - Robust tokenization, keywords (`int`, `return`, `void`), identifiers, literals, operators, comments, and line/column diagnostics.
- **Milestone 3: Recursive Descent Parser & AST**
  - Parse functions, parameters, variable declarations, and arithmetic expressions with operator precedence.
- **Milestone 4: Semantic Analysis**
  - Symbol tables, scope checking, type validation, and reporting undeclared identifiers.
- **Milestone 5: Go Code Generation**
  - Emit idiomatic Go code; handle entry-point semantics (`cMain()` vs Go `main()`).
- **Milestone 6: Control Flow & Scope Expansion**
  - Add `if/else`, loops, nested scopes, and multiple functions.
- **Milestone 7: Differential Compatibility Testing**
  - Automate test suite comparing outputs: `gcc output.exe` vs `go run output.go`.
- **Milestone 8: Intermediate Representation & Custom Bytecode VM**
  - Decouple the frontend from the Go backend via IR, opening the door for a custom bytecode virtual machine.

---

## 🛠️ Development Environment & Prerequisites

### Required Tools (Windows)
1. **C Toolchain:** GCC via [MSYS2](https://www.mingw-w64.org/getting-started/msys2/) (UCRT64 or MINGW64) or MinGW-w64.
2. **Go Toolchain:** Official Go compiler ([https://go.dev/doc/install](https://go.dev/doc/install)).
3. **Version Control:** Git.

Check installed versions:
```powershell
gcc --version
go version
git --version
```

---

## 🔨 Building & Running

### 1. Build the C2Go Compiler
```powershell
gcc -Wall -Wextra -g3 -Iinclude compiler/main.c compiler/arena.c -o compiler.exe
```

### 2. Transpile a C Program
```powershell
.\compiler.exe examples\hello.c -o output\main.go
```

### 3. Build & Run the Generated Go Code
```powershell
go run output\main.go
```

---

## 📐 Design Principles

1. **Correctness before breadth:** A small, reliable compiler is vastly superior to a buggy, feature-bloated one.
2. **Real parsing, not regex substitution:** C syntax demands true syntactic and precedence parsing.
3. **Explicit semantics:** C and Go have distinct type rules, overflow behaviors, and exit mechanics; translation must bridge these intentionally.
4. **Separation of concerns:** Lexing, parsing, analysis, and code generation remain isolated modules.
5. **Fast memory management:** Employ arena-based allocation for simple, leak-free compiler passes.
