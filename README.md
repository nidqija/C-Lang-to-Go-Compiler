# 🚀 C2Go: A C-to-Go Source-to-Source Compiler

<p align="center">
  <img src="https://img.shields.io/badge/Language-C11-blue.svg" alt="C11">
  <img src="https://img.shields.io/badge/Target-Go%201.20+-00ADD8.svg" alt="Go">
  <img src="https://img.shields.io/badge/Frontend-SvelteKit%205%20%2B%20Tailwind-FF3E00.svg" alt="SvelteKit 5">
  <img src="https://img.shields.io/badge/Status-Milestones%201--3%2C%205--8%20Completed-brightgreen.svg" alt="Status">
  <img src="https://img.shields.io/badge/Homelab-Self--Hosted%20Compiler-blueviolet.svg" alt="Self-Hosted Homelab">
  <img src="https://img.shields.io/badge/Remote-Tailscale%20%2B%20Termius-9cf.svg" alt="Tailscale + Termius">
  <img src="https://img.shields.io/badge/Dependencies-Zero%20External-orange.svg" alt="Zero Dependencies">
</p>

**C2Go** is an independent, zero-dependency source-to-source compiler (transpiler) written entirely in **C11** from scratch. It translates a defined subset of **C** source code into idiomatic, syntactically valid **Go** code that compiles and runs with the official Go toolchain.

Beyond the CLI compiler, C2Go features a **self-hosted Web Playground REPL** (built with SvelteKit 5 and Tailwind CSS) and a **lightweight Go execution engine**, allowing real-time code transpilation, AST introspection, and program execution directly from desktop browsers or mobile phones over an encrypted **Tailscale mesh network**.

The compiler core is built without external parser generators or compiler frameworks (no Lex/Yacc, Bison, or LLVM). Every phase—from arena-based dynamic memory allocation to lexical analysis, recursive descent parsing, AST construction, and Go code generation—is engineered ground-up in standard C.

---

## 📑 Table of Contents
- [Core Concept](#-core-concept)
- [Working Demonstrations](#-working-demonstrations)
  - [1. For Loop & Variable Iteration](#1-for-loop--variable-iteration)
  - [2. Binary Arithmetic & PEMDAS Precedence](#2-binary-arithmetic--pemdas-precedence)
- [Web Playground REPL (Desktop & Mobile)](#-web-playground-repl-desktop--mobile)
- [Technology Stack & Infrastructure](#️-technology-stack--infrastructure)
- [Compiler Architecture & Deep Dive](#-compiler-architecture--deep-dive)
  - [1. Arena Allocator (`arena.c` / `arena.h`)](#1-arena-allocator-arenac--arenah)
  - [2. Lexical Analyzer (`lexer.c` / `lexer.h`)](#2-lexical-analyzer-lexerc--lexerh)
  - [3. Recursive Descent Parser (`parser.c` / `parser.h`)](#3-recursive-descent-parser-parserc--parserh)
  - [4. Abstract Syntax Tree (`ast.h` / `ast.c`)](#4-abstract-syntax-tree-asth--astc)
  - [5. Go Code Generator (`codegen.c` / `codegen.h`)](#5-go-code-generator-codegenc--codegenh)
  - [6. Go Web Server Backend (`web-server/server.go`)](#6-go-web-server-backend-web-serverservergo)
- [Project Structure](#-project-structure)
- [Implementation Roadmap](#-implementation-roadmap)
- [Building & Running](#-building--running)
- [Homelab & Mobile Deployment (Tailscale)](#-homelab--mobile-deployment-tailscale)
- [Deployment & Operations Runbook (DEPLOYMENT.md)](DEPLOYMENT.md)
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

## 🌟 Working Demonstrations

### 1. For Loop & Variable Iteration

#### Input: C Source (`examples/hello.c`)
```c
#include <stdio.h>

int main() {
    printf("Hello from C2Go!\n");

    for (int i = 0; i < 4; i++) {
        printf("Hello world\n");
    }

    return 0;
}
```

#### Generated Output: Go Source (`output/main.go`)
```go
package main

import "fmt"
import "os"

func cMain() int32{
	_ = fmt.Printf
	fmt.Printf("Hello from C2Go!\n")
	for i := int32(0); i < 4; i++{
		fmt.Printf("Hello world\n")
	}
	return 0
}

func main(){
	_ = fmt.Printf
	os.Exit(int(cMain()))
}
```

#### Execution Output:
```text
$ go run output/main.go
Hello from C2Go!
Hello world
Hello world
Hello world
Hello world
```

---

### 2. Binary Arithmetic & PEMDAS Precedence

#### Input: C Source
```c
int main() {
    return 50 - 10 / 2 + 3 * 4;
}
```

#### Generated Output: Go Source
```go
package main

import "fmt"
import "os"

func cMain() int32{
	_ = fmt.Printf
	return 50 - 10 / 2 + 3 * 4
}

func main(){
	_ = fmt.Printf
	os.Exit(int(cMain()))
}
```
*Evaluating `50 - (10/2) + (3*4) = 50 - 5 + 12 = 57`, preserved exactly through AST precedence cascades.*

---

## 🌐 Web Playground REPL (Desktop & Mobile)

C2Go includes an interactive Web Playground that lets anyone write C code, transpile it to Go, and view runtime execution in a browser.

- **Stack:** Built with modern **SvelteKit 5** (utilizing reactive `$state` runes) and **Tailwind CSS**.
- **Mobile-Responsive:** Automatically adapts from a side-by-side IDE split on desktop to a touch-optimized stacked layout on mobile screens with scrollable tabs and zero unwanted iOS auto-zoom.
- **Three-Way Output Inspection:**
  - 🖥️ **Console Output:** Real-time program output (`stdout` & `stderr`) with process exit status badges.
  - 🐹 **Generated Go Code:** Syntax-highlighted Go code emitted directly by the C2Go backend.
  - 🌳 **AST Tree:** Visualized Abstract Syntax Tree showing declarations, loop structures, and expressions.
- **One-Tap Presets:** Quick-load buttons for `For Loop`, `PEMDAS Math`, and `Hello World`.

---

## 🛠️ Technology Stack & Infrastructure

### 1. Compiler Core (Languages & Systems Programming)
| Layer | Technology | Details |
|---|---|---|
| **Implementation Language** | **C11 (ISO/IEC 9899:2011)** | Zero third-party dependencies, strict standard library only |
| **Target Output Language** | **Go (Golang 1.18+)** | Clean, formatted Go code compiled via official `go` toolchain |
| **C Compilers** | **GCC & Clang** | Built with `-Wall -Wextra -std=c11` for cross-platform binaries |
| **Memory Architecture** | **Linear Bump Arena** | Custom 4MB contiguous memory arena, $O(1)$ allocation, zero leaks |
| **Parsing Technique** | **Recursive Descent** | Handcrafted top-down LL(1) grammar parser with single-token lookahead |
| **Operator Precedence** | **Cascading Hierarchy** | Multi-level precedence parser handling `+`, `-`, `*`, `/`, `<`, `<=`, `>`, `>=` |
| **Syntax Tree** | **Tagged Union AST** | Explicit node hierarchies with indentation-aware tree visualization |

### 2. Web Engine & Homelab Infrastructure
| Component | Technology | Role |
|---|---|---|
| **Web UI Frontend** | **SvelteKit 5 & Tailwind CSS** | Mobile-responsive dark-mode IDE playground & REPL |
| **Execution Backend** | **Go (`server.go`)** | Lightweight HTTP server handling compilation & process execution on port `8080` |
| **Service Supervisor** | **Bash (`start_playground.sh`)** | All-in-one homelab startup script with auto-build & process management |
| **Host Operating System** | **Debian GNU/Linux** | Self-hosted homelab compute node (WSL2 / native Debian) |
| **Mesh Networking** | **Tailscale (WireGuard)** | Encrypted peer-to-peer overlay network with static `100.x.y.z` IP |
| **Remote Access** | **OpenSSH Server (`sshd`)** | Secure remote shell listening on port 22 with Ed25519 keypair |
| **Mobile Client** | **Termius (Android / iOS)** | Touch-friendly mobile terminal with 1-tap snippet macros |

---

## 🏗️ Compiler Architecture & Deep Dive

The compiler executes as a sequential multi-stage pipeline:

```text
 C Source Text
      │
      ▼
 ┌─────────┐
 │  Lexer  │ ────► Token Stream (Keywords, Literals, Identifiers, Operators)
 └────┬────┘
      │
      ▼
 ┌─────────┐
 │ Parser  │ ────► Abstract Syntax Tree (AST tagged unions)
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
- **Token Set:**
  - Keywords: `int`, `return`, `void`, `for`.
  - Literals: `TOKEN_INT_LIT`, `TOKEN_STRING_LIT`.
  - Operators & Delimiters: `=`, `+`, `-`, `*`, `/`, `++`, `<`, `<=`, `>`, `>=`, `(`, `)`, `{`, `}`, `;`.
- **Features:** Source line/column tracking for compiler errors, automatic whitespace/comment stripping, and C preprocessor directive skipping (`#include`).

### 3. Recursive Descent Parser (`parser.c` / `parser.h`)
- **Purpose:** Analyzes the token stream against the grammatical rules of the supported C subset.
- **Precedence Hierarchy:** Implements cascading precedence for mathematical expressions:
  - `parse_expression()`: Handles addition (`+`) and subtraction (`-`).
  - `parse_multi_divide()`: Handles multiplication (`*`) and division (`/`).
  - `parse_primary()`: Handles base literals, identifiers, variable references, parenthesized sub-expressions, and function calls.
- **Control Flow & Declarations:**
  - `parse_statement()`: Dispatches variable declarations (`int i = 0;`), `for` loop statements, function calls, and `return` statements.
  - `NODE_FOR_STMT`: Parses `init`, `cond`, `post`, and statement body lists.

### 4. Abstract Syntax Tree (`ast.h` / `ast.c`)
- **Structure:** Defined using tagged unions (`NodeKind` + `union`):
  - `NODE_FUNCTION_DECL`: Function declaration with parameter and body statement lists.
  - `NODE_VAR_DECL`: Local variable declarations with identifier slice and initializer expression.
  - `NODE_VAR_REF`: Identifier variable lookups.
  - `NODE_FOR_STMT`: Loops holding `init`, `cond`, `post`, and body statements.
  - `NODE_INC_EXPR`: Increment expressions (`i++`).
  - `NODE_BINARY_EXPR`: Left and right subtrees linked by binary operators (`+`, `-`, `*`, `/`, `<`).
  - `NODE_RETURN_STMT` & `NODE_CALL_STMT`: Return statements and function calls (`printf`).
- **Tree Visualizer (`ast_print`):** Indentation-aware tree inspector for debugging compiler output.

### 5. Go Code Generator (`codegen.c` / `codegen.h`)
- **Semantic Bridging:**
  - **Function Return Mapping:** C `main()` returns an exit integer; Go `main()` cannot return values. The generator wraps C logic into `func cMain() int32` and calls `os.Exit(int(cMain()))` from Go `main()`.
  - **Loop Translation:** Translates C `for (int i = 0; i < 4; i++)` into idiomatic Go `for i := int32(0); i < 4; i++ { ... }`.
  - **Standard Library:** Maps C standard I/O calls (`printf`) directly to Go's `fmt.Printf`.

### 6. Go Web Server Backend (`web-server/server.go`)
- Self-contained Go HTTP server listening on port `8080`.
- Implements `/api/run` POST route to execute `c2go transpile` and `go run` dynamically with JSON responses.

---

## 📂 Project Structure

```text
C-And-Go-Compiler/
├── compiler/               # Core Compiler Implementation (C sources)
│   ├── main.c              # CLI Driver & Pipeline Coordinator
│   ├── arena.c             # Fast Bump Memory Arena Allocator
│   ├── lexer.c             # Lexical Analyzer & Tokenizer
│   ├── parser.c            # Recursive Descent Syntax Analyzer
│   ├── ast.c               # AST Visualizer & Formatting
│   └── codegen.c           # Go Code Generator Backend
├── include/                # Compiler Header Interfaces
│   ├── arena.h             # Memory Arena Interface
│   ├── lexer.h             # Token Types & Lexer State
│   ├── parser.h            # Parser Prototypes & State
│   ├── ast.h               # AST Node Tagged Unions
│   └── codegen.h           # Code Generator API
├── web/                    # SvelteKit 5 + Tailwind Web Playground
│   ├── src/
│   │   └── routes/
│   │       ├── +page.svelte# Mobile-Responsive IDE & REPL
│   │       └── layout.css  # Tailwind styling
│   ├── package.json        # Frontend Dependencies
│   └── vite.config.ts      # Vite configuration & API proxy
├── web-server/             # Web API Backend
│   └── server.go           # Go Execution Engine (:8080)
├── examples/               # Test C Programs
│   └── hello.c             # Loop & Expression Test Suite
├── output/                 # Transpiled Output Directory
│   └── main.go             # Emitted Go Program
├── start_playground.sh     # Homelab Startup & Service Manager Script
├── test_deploy.ps1         # Windows Deployment Automation Script
├── DEPLOYMENT.md           # Homelab Operations & Mobile Runbook
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
| **Milestone 6** | Expressions & Operators | ✅ Completed | Binary arithmetic (`+`, `-`, `*`, `/`) with PEMDAS precedence |
| **Milestone 7** | Local Variables | ✅ Completed | Variable declarations (`int i = 0;`), references, and `++` |
| **Milestone 8** | Control Flow (`for`) | ✅ Completed | C `for` loops transpiled to idiomatic Go `for` loops |
| **Milestone 9** | Web Playground REPL | ✅ Completed | SvelteKit 5 mobile-responsive UI with live AST & Go output |
| **Milestone 10**| Branching & While Loops | ⚪ Planned | Support for `if`/`else` and `while` loop statements |
| **Milestone 11**| Custom Bytecode VM | ⚪ Future | Custom IR and stack-based virtual machine in C |

---

## 🔨 Building & Running

### 1. Local CLI Compiler

#### Windows (PowerShell)
```powershell
# 1. Compile C2Go CLI
gcc -Wall -Wextra -std=c11 -Iinclude compiler/main.c compiler/arena.c compiler/lexer.c compiler/ast.c compiler/parser.c compiler/codegen.c -o c2go.exe

# 2. Transpile C to Go
.\c2go.exe transpile examples/hello.c -o output/main.go

# 3. Run Emitted Go Code
go run output/main.go
```

#### Linux / Debian / macOS (Bash)
```bash
# 1. Compile C2Go CLI
gcc -Wall -Wextra -std=c11 -Iinclude compiler/main.c compiler/arena.c compiler/lexer.c compiler/ast.c compiler/parser.c compiler/codegen.c -o c2go

# 2. Transpile C to Go
./c2go transpile examples/hello.c -o output/main.go

# 3. Run Emitted Go Code
go run output/main.go
```

---

## 🏠 Homelab & Mobile Deployment (Tailscale)

You can run the full C2Go Web Playground as a self-hosted service and access it directly from your phone over Tailscale.

### 1-Command Startup (`start_playground.sh`)
On your Debian host / homelab server:
```bash
chmod +x start_playground.sh
./start_playground.sh
```

This single command will:
1. Compile the `c2go` binary if missing.
2. Install frontend dependencies (`npm install`) if needed.
3. Launch the Go API backend (`server.go`) in the background on port `8080`.
4. Detect your **Tailscale IP** (`tailscale ip -4`).
5. Launch the SvelteKit frontend bound to `0.0.0.0:5173`.

### Accessing From Your Phone
1. Connect to **Tailscale** on your phone (iOS / Android).
2. Open your mobile browser and navigate to:
   ```text
   http://<YOUR_TAILSCALE_IP>:5173
   ```
3. Type C code, click presets, and tap **▶ Run** to execute code live on your homelab!

---

## 📐 Design Principles

1. **Correctness Before Breadth:** Prioritize rock-solid parsing and valid code generation over rushed feature sets.
2. **Zero Dependencies:** Pure standard library C11. No third-party libraries, Lex/Flex, Bison, or LLVM.
3. **Linear Memory Management:** Arena allocators guarantee deterministic memory usage with instantaneous teardown.
4. **Observable Equivalence:** The emitted Go code must preserve the observable behavior and exit codes of the original C program.
5. **Clear Diagnostic Locations:** Every error reports exact line and column numbers for fast developer debugging.
