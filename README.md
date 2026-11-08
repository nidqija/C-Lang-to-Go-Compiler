# 🚀 C2Go: A C-to-Go Source-to-Source Compiler

<p align="center">
  <img src="https://img.shields.io/badge/Language-C11-blue.svg" alt="C11">
  <img src="https://img.shields.io/badge/Target-Go%201.20+-00ADD8.svg" alt="Go">
  <img src="https://img.shields.io/badge/Status-Milestones%201--3%2C%205%20Completed-brightgreen.svg" alt="Status">
  <img src="https://img.shields.io/badge/Dependencies-Zero%20External-orange.svg" alt="Zero Dependencies">
</p>

**C2Go** is an independent, zero-dependency source-to-source compiler (transpiler) written entirely in **C11** from scratch. It translates a defined subset of **C** source code into idiomatic, syntactically valid **Go** code that compiles and runs with the official Go toolchain.

The project is built without external parser generators or compiler frameworks (no Lex/Yacc, Bison, or LLVM). Every phase—from arena-based dynamic memory allocation to lexical analysis, recursive descent parsing, AST construction, and Go code generation—is engineered ground-up in C.

---

## 📑 Table of Contents
- [Core Concept](#-core-concept)
- [Working Demonstration](#-working-demonstration)
- [Compiler Architecture & Deep Dive](#-compiler-architecture--deep-dive)
  - [1. Arena Allocator (`arena.c` / `arena.h`)](#1-arena-allocator-arenac--arenah)
  - [2. Lexical Analyzer (`lexer.c` / `lexer.h`)](#2-lexical-analyzer-lexerc--lexerh)
  - [3. Recursive Descent Parser (`parser.c` / `parser.h`)](#3-recursive-descent-parser-parserc--parserh)
  - [4. Abstract Syntax Tree (`ast.h` / `ast.c`)](#4-abstract-syntax-tree-asth--astc)
  - [5. Go Code Generator (`codegen.c` / `codegen.h`)](#5-go-code-generator-codegenc--codegenh)
  - [6. CLI Driver (`main.c`)](#6-cli-driver-mainc)
- [Project Structure](#-project-structure)
- [Implementation Roadmap](#-implementation-roadmap)
- [Building & Running](#-building--running)
- [Design Principles](#-design-principles)

---

## 💡 Core Concept

```text
 ┌──────────────────────┐
 │   C Source (.c)      │
 └──────────┬───────────┘
            │
            ▼
 ┌────────────────────────────────────────────────────────┐
 │                 C2Go Transpiler (in C)                 │
 │  [Lexer] ➔ [Parser & AST] ➔ [C-to-Go Code Generator]   │
 └──────────────────────────┬─────────────────────────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │  Go Source (.go)     │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │ Official Go Compiler │
                 │      (go run)        │
                 └──────────┬───────────┘
                            │
                            ▼
                 ┌──────────────────────┐
                 │  Native Executable   │
                 └──────────────────────┘
```

---

## 🌟 Working Demonstration

### Input: C Source (`examples/hello.c`)
```c
#include <stdio.h>

int main() {
    printf("Hello world\n");
    return 0;
}
```

### Generated Output: Go Source (`output/main.go`)
```go
package main

import "fmt"
import "os"

func cMain() int32{
	fmt.Printf("Hello world\n")
	return 0
}

func main(){
	os.Exit(int(cMain()))
}
```

### Execution Output:
```text
$ go run output/main.go
Hello world
```

---

## 🏗️ Compiler Architecture & Deep Dive

The compiler executes as a sequential multi-stage pipeline:

```text
 C Source Text
      │
      ▼
 ┌─────────┐
 │  Lexer  │ ────► Token Stream (with line & column metadata)
 └────┬────┘
      │
      ▼
 ┌─────────┐
 │ Parser  │ ────► Abstract Syntax Tree (AST)
 └────┬────┘
      │
      ▼
 ┌─────────┐
 │ Codegen │ ────► Valid, formatted Go Source (.go)
 └─────────┘
```

### 1. Arena Allocator (`arena.c` / `arena.h`)
- **Purpose:** Eliminates complex manual memory management and fragmentation during compilation passes.
- **Design:** Implements an 8-byte aligned linear bump allocator (`arena_alloc`). 
- **Efficiency:** Allocations are $O(1)$ operations that advance an internal byte offset into a preallocated 4MB memory buffer.
- **Clean Teardown:** All AST nodes, token arrays, and string references are automatically reclaimed in one operation (`arena_free`) when compilation completes, guaranteeing zero memory leaks.

### 2. Lexical Analyzer (`lexer.c` / `lexer.h`)
- **Purpose:** Scans raw source code bytes into meaningful syntactic tokens.
- **Token Set:** Keywords (`int`, `return`, `void`), identifiers, numbers (`TOKEN_INT_LIT`), string literals (`TOKEN_STRING_LIT`), and operators/punctuation (`(`, `)`, `{`, `}`, `;`).
- **Features:**
  - Full source location tracking (line number and column offset) for actionable error diagnostics.
  - Automatic whitespace and comment (`//`) stripping.
  - Preprocessor line skipping (`#...` lines like `#include <stdio.h>` are ignored cleanly).
  - String escape sequence handling (`\"`, `\n`).

### 3. Recursive Descent Parser (`parser.c` / `parser.h`)
- **Purpose:** Analyzes the token stream against the grammatical rules of the C language.
- **Mechanism:** Implements a top-down recursive descent parser with single-token lookahead (`parser->current`).
- **Safety:** Utilizes the strict `expect()` utility to enforce grammatical requirements and abort cleanly on syntax errors with precise line/column locations.
- **Grammar Supported:**
  - Translation units & function declarations (`int main()`, `int main(void)`).
  - Statement parsing: Return statements (`return <expr>;`) and Expression statements (`printf(...);`).
  - Expression parsing: Numeric literals, string literals, and function calls with argument lists.

### 4. Abstract Syntax Tree (`ast.h` / `ast.c`)
- **Purpose:** In-memory, strongly typed tree hierarchy representing the program's structure.
- **Structure:** Defined using a tagged union (`NodeKind` + `union`):
  - `NODE_FUNCTION_DECL`: Function name, return type, and statement list (`body_count`).
  - `NODE_RETURN_STMT`: Return statements holding an expression node.
  - `NODE_EXPR_STMT`: Statements composed of evaluated expressions.
  - `NODE_CALL_STMT`: Function calls holding callee name and argument lists (`args`).
  - `NODE_INT_LITERAL`: 64-bit integer values (`int64_t`).
  - `NODE_STRING_LITERAL`: Raw string pointer and length slice.
- **Inspection:** Includes an indentation-aware tree visualizer (`ast_print`) for debug introspection:
  ```text
  --- Abstract Syntax Tree ---
  [Decl: Function] name='main' body_statements=2
    Statement[0]:
      [Stmt: Expression]
        [Expr: Call] func='printf' args=1
          [Expr: StringLiteral] value="Hello world\n"
    Statement[1]:
      [Stmt: Return]
        [Expr: IntLiteral] value=0
  ----------------------------
  ```

### 5. Go Code Generator (`codegen.c` / `codegen.h`)
- **Purpose:** Serializes the AST into readable, valid Go code.
- **Semantic Bridging:** Bridges key semantic differences between C and Go:
  - **Program Entrypoint:** In C, `main()` returns an `int` exit code. In Go, `main()` cannot return values. The code generator wraps C logic inside `func cMain() int32` and emits an outer Go `func main() { os.Exit(int(cMain())) }`, accurately preserving exit status semantics.
  - **Standard Library Translation:** Translates standard C functions to their Go equivalents (e.g., C `printf(...)` ➔ Go `fmt.Printf(...)`).
  - **Automatic Import Management:** Emits required Go packages (`import "fmt"`, `import "os"`).

### 6. CLI Driver (`main.c`)
- **Purpose:** Command-line driver coordinating file I/O, memory arena initialization, lexing, parsing, and writing the transpiled output.

---

## 📂 Project Structure

```text
C-And-Go-Compiler/
├── compiler/               # Core Compiler Implementation (C sources)
│   ├── main.c              # CLI Driver & Pipeline Coordinator
│   ├── arena.c             # Fast Bump Memory Arena
│   ├── lexer.c             # Lexical Analyzer & Tokenizer
│   ├── parser.c            # Recursive Descent Syntax Analyzer
│   ├── ast.c               # AST Visualizer & Formatting
│   ├── codegen.c           # Go Code Generator Backend
│   └── sema.c              # Semantic Analyzer (Planned)
├── include/                # Compiler Header Interfaces
│   ├── arena.h             # Memory Arena Interface
│   ├── lexer.h             # Token Types & Lexer State
│   ├── parser.h            # Parser Prototypes & State
│   ├── ast.h               # AST Node Tagged Unions
│   ├── codegen.h           # Code Generator API
│   └── sema.h              # Semantic Analysis API (Planned)
├── examples/               # Test C Programs
│   └── hello.c             # "Hello World" & Exit Code Test
├── output/                 # Transpiled Output Directory
│   └── main.go             # Emitted Go Program
└── README.md
```

---

## 📍 Implementation Roadmap

| Milestone | Component | Status | Description |
|---|---|:---:|---|
| **Milestone 1** | Minimal Proof-of-Concept | ✅ Completed | End-to-end pipeline: parse C, emit Go, and execute cleanly |
| **Milestone 2** | Lexer & Tokenizer | ✅ Completed | Keywords, identifiers, numbers, strings, comments & locations |
| **Milestone 3** | Recursive Descent Parser & AST | ✅ Completed | Functions, return statements, function calls, and expressions |
| **Milestone 4** | Semantic Analysis | 🟡 Planned | Type checker, symbol tables, scope resolution |
| **Milestone 5** | Go Code Generation | ✅ Completed | Functional Go backend, `fmt.Printf`, and `os.Exit` mappings |
| **Milestone 6** | Expressions & Operators | 🟡 Next Up | Arithmetic expressions (`+`, `-`, `*`, `/`) with precedence |
| **Milestone 7** | Local Variables | 🟡 Next Up | Variable declarations (`int x = 10;`) and assignments |
| **Milestone 8** | Control Flow | ⚪ Planned | Support for `if`/`else`, `for`, and `while` loops |
| **Milestone 9** | Custom Bytecode VM | ⚪ Future | Custom IR and stack-based virtual machine in C |

---

## 🔨 Building & Running

### Prerequisites
- **GCC / Clang** (supporting C11, e.g. MinGW-w64 on Windows, GCC on Linux/macOS)
- **Go 1.18+** (to run generated output)

### 1. Compile the C2Go Transpiler
```powershell
gcc -Wall -Wextra -std=c11 -Iinclude compiler/main.c compiler/arena.c compiler/lexer.c compiler/ast.c compiler/parser.c compiler/codegen.c -o c2go.exe
```

### 2. Transpile C to Go
```powershell
.\c2go.exe transpile examples/hello.c -o output/main.go
```

### 3. Run the Transpiled Go Code
```powershell
go run output/main.go
```

---

## 📐 Design Principles

1. **Correctness Before Breadth:** Prioritize rock-solid parsing and valid code generation over rushed feature sets.
2. **Zero Dependencies:** Pure standard library C11. No third-party libraries, Lex/Flex, Bison, or LLVM.
3. **Linear Memory Management:** Arena allocators guarantee deterministic memory usage with instantaneous teardown.
4. **Observable Equivalence:** The emitted Go code must preserve the observable behavior and exit codes of the original C program.
5. **Clear Diagnostic Locations:** Every error reports exact line and column numbers for fast developer debugging.
