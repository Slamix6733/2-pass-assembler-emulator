# Two-Pass Assembler & Virtual Machine Emulator

An end-to-end implementation of a **Two-Pass Assembler** and an interactive **Instruction Set Architecture (ISA) Emulator** for a 32-bit load-store architecture. Developed as part of the Digital Logic & Computer Organization (DLCO / CS2102) course at IIT Patna.

---

## Table of Contents

- [Overview](#overview)
- [Architecture & Instruction Set (ISA)](#architecture--instruction-set-isa)
  - [Register Set](#register-set)
  - [Instruction Encoding](#instruction-encoding)
  - [Instruction Set Table](#instruction-set-table)
  - [Assembler Directives](#assembler-directives)
- [System Components](#system-components)
  - [1. Two-Pass Assembler (`assemble.cpp`)](#1-two-pass-assembler-assemblecpp)
  - [2. Interactive Virtual Machine Emulator (`emulator.cpp`)](#2-interactive-virtual-machine-emulator-emulatorcpp)
- [Project Structure](#project-structure)
- [Prerequisites & Build Instructions](#prerequisites--build-instructions)
- [Usage Guide](#usage-guide)
  - [Assembling Source Code](#assembling-source-code)
  - [Running the Emulator](#running-the-emulator)
- [Test Cases & Verification](#test-cases--verification)
- [Author & Academic Declaration](#author--academic-declaration)

---

## Overview

This repository provides a complete toolchain for assembling and simulating programs written in a custom assembly language:

1. **Two-Pass Assembler**: Converts assembly source files (`.asm`) into binary machine-code object files (`.o`), annotated listing files (`.lst`), and diagnostics logs (`.log`).
2. **Virtual Machine Emulator**: Loads binary object files into simulated memory and executes instructions with cycle-by-cycle tracing, memory inspection, and memory read/write tracking.

---

## Architecture & Instruction Set (ISA)

### Register Set

The virtual CPU consists of four 32-bit registers:

| Register | Name | Role |
| :---: | :--- | :--- |
| **`A`** | Accumulator | Primary register used for arithmetic, logic, and memory operations. |
| **`B`** | Temporary / Secondary | Holds previous accumulator values during loads, pushes, and binary operations. |
| **`PC`** | Program Counter | Holds the memory address of the next instruction to execute. |
| **`SP`** | Stack Pointer | Points to the current top of the execution stack in memory. |

### Instruction Encoding

All instructions are packed into fixed **32-bit words**:

```text
 31                                 8 7           0
+------------------------------------+-------------+
|        24-bit Signed Offset        | 8-bit Opcode|
+------------------------------------+-------------+
```

- **Bits 0–7 (8 bits):** Operation Code (Opcode).
- **Bits 8–31 (24 bits):** Signed 24-bit immediate operand or relative address offset. Negative values are sign-extended to 32 bits upon execution.

### Instruction Set Table

| Opcode (Hex) | Mnemonic | Operand Type | Semantics & Description |
| :---: | :--- | :--- | :--- |
| `00` | `ldc` | Value | `B = A; A = value;` (Load constant into accumulator) |
| `01` | `adc` | Value | `A = A + value;` (Add constant to accumulator) |
| `02` | `ldl` | Offset | `B = A; A = mem[SP + offset];` (Load local from stack) |
| `03` | `stl` | Offset | `mem[SP + offset] = A; A = B;` (Store accumulator to stack) |
| `04` | `ldnl` | Offset | `A = mem[A + offset];` (Load non-local from heap/address) |
| `05` | `stnl` | Offset | `mem[A + offset] = B;` (Store `B` into heap address `A + offset`) |
| `06` | `add` | None | `A = B + A;` (Add `B` and `A`) |
| `07` | `sub` | None | `A = B - A;` (Subtract `A` from `B`) |
| `08` | `shl` | None | `A = B << A;` (Logical shift left `B` by `A` bits) |
| `09` | `shr` | None | `A = B >> A;` (Logical shift right `B` by `A` bits) |
| `0a` | `adj` | Value | `SP = SP + value;` (Adjust stack pointer) |
| `0b` | `a2sp` | None | `SP = A; A = B;` (Transfer `A` to `SP`) |
| `0c` | `sp2a` | None | `B = A; A = SP;` (Transfer `SP` to `A`) |
| `0d` | `call` | Offset | `B = A; A = PC; PC = PC + offset;` (Subroutine call) |
| `0e` | `return` | None | `PC = A; A = B;` (Subroutine return) |
| `0f` | `brz` | Offset | If `A == 0`, `PC = PC + offset;` (Branch if zero) |
| `10` | `brlz` | Offset | If `A < 0`, `PC = PC + offset;` (Branch if less than zero) |
| `11` | `br` | Offset | `PC = PC + offset;` (Unconditional relative branch) |
| `12` | `HALT` | None | Stop execution |

### Assembler Directives

- `data <value>`: Allocates a 32-bit word initialized with `<value>`.
- `SET <value>`: Declares a label with a symbolic constant value instead of referencing the current program counter address.

---

## System Components

### 1. Two-Pass Assembler (`assemble.cpp`)

The assembler processes assembly files in two distinct phases:

- **Pass 1**:
  - Tokenizes lines and strips comments (initiated with `;`).
  - Identifies and catalogues label definitions, resolving addresses and detecting duplicates.
  - Detects syntax errors, bogus mnemonics, missing labels, unexpected operands, or malformed numerical representations.
  - Produces an execution diagnostics log (`.log`). If any errors occur, execution halts before code generation.
- **Pass 2**:
  - Resolves branch targets and label references relative to the Program Counter.
  - Generates the binary object code file (`.o`).
  - Generates the formatted listing file (`.lst`), aligning `[Program Counter] [Hex Machine Code] [Source Code]`.

### 2. Interactive Virtual Machine Emulator (`emulator.cpp`)

The emulator reads the generated binary `.o` file into virtual memory (array of $2 \times 10^7$ words) and provides a terminal-based interactive control menu:

- **Option 1 (Trace):** Executes program step-by-step, logging every instruction along with register states (`PC`, `SP`, `A`, `B`) and total cycles executed. Protects against infinite loops (threshold $2^{24}$ cycles).
- **Option 2 (Display ISA):** Displays the opcode mapping and mnemonic summary.
- **Option 3 (Reset Flags):** Resets all core registers (`A = B = PC = SP = 0`).
- **Option 4 (Memory Dump Before Execution):** Dumps initial memory state in formatted 32-bit hexadecimal blocks.
- **Option 5 (Memory Dump After Execution):** Dumps final memory state after program termination.
- **Option 6 (Show Memory Reads):** Lists all memory read operations and values encountered during execution.
- **Option 7 (Show Memory Writes):** Lists all memory write operations detailing memory address, previous value, and updated value.
- **Option 8+ (Exit):** Closes the emulator.

---

## Project Structure

```text
.
├── assemble.cpp            # Two-pass assembler source code
├── emulator.cpp            # Virtual machine emulator source code
├── data.txt                # Opcode table and operand metadata for assembler
├── command.txt             # ISA reference table for emulator
├── claims_asm.txt          # Authorship and assembler test summary
├── claims_emu.txt          # Authorship and emulator test summary
├── Given test cases/       # Course standard test benchmarks
│   ├── test1.asm           # Basic operations and label jumps
│   ├── test1.lst           # Generated assembly listing
│   ├── test1.o             # Binary object file
│   ├── test1.log           # Assembler diagnostics log
│   ├── test2.asm           # Error detection test suite (bogus mnemonics, syntax errors)
│   ├── test2.log           # Diagnostics log showing caught errors
│   ├── test3.asm           # Control flow & branch logic tests
│   └── test4.asm           # Data allocation and set directive tests
└── Additional Test cases/  # Custom assembly programs and verification suites
    ├── add/                # Integer addition program
    ├── area/               # Geometric calculation program
    ├── array_sum/          # Array iteration and summation
    ├── branch_show/        # Conditional branching demonstration
    └── bubble_sort/        # Bubble sort algorithm implementation
```

---

## Prerequisites & Build Instructions

### Prerequisites

- A C++ compiler supporting **C++17** or later (e.g., GCC / `g++` or Clang).
- Operating System: Linux, macOS, or Windows (via WSL / MinGW).

### Compilation

Compile both utilities using `g++`:

```bash
# Compile the Assembler
g++ -std=c++17 assemble.cpp -o assemble

# Compile the Emulator
g++ -std=c++17 emulator.cpp -o emulator
```

---

## Usage Guide

### Assembling Source Code

Make sure `data.txt` is in your working directory. Run the assembler and provide the path to your assembly file:

```bash
./assemble
```

**Prompt Example:**
```text
Enter the name of file you wnat to give as input
Given test cases/test1.asm
```

**Outputs Produced:**
- `<filename>.lst`: Formatted listing file with addresses and machine code.
- `<filename>.o`: Binary executable object code.
- `<filename>.log`: Status log (reports warnings or errors).

### Running the Emulator

Make sure `command.txt` is in your working directory. Run the emulator and specify the generated `.o` file:

```bash
./emulator
```

**Prompt Example:**
```text
Enter the File name for which you want to run emulator (with extension):
Given test cases/test1.o
```

You will then be presented with the interactive menu:

```text
MENU
1: Trace
2: Display ISA
3: Reset Flags
4: Show Memory Dump (before)
5: Show Memory Dump (after)
6: Show Memory Reads
7: Show Memory Writes
8+: Exit

Choose an option:
1
```

---

## Test Cases & Verification

The suite includes multiple test programs demonstrating error resilience and algorithmic execution:

- **`test1.asm`**: Verifies constant loading (`ldc`), labels, backward/forward branching (`br`).
- **`test2.asm`**: Verifies assembler error handling. Successfully catches:
  - Duplicate label definitions
  - Undefined label references
  - Bogus/invalid mnemonics
  - Unexpected operands for zero-operand instructions
  - Extra trailing tokens
- **`test3.asm` & `test4.asm`**: Complex branching, memory read/write operations, and `data`/`SET` directives.
- **`bubble_sort.asm`**: In-place sorting of an array loaded into memory, exercising memory reads (`ldnl`), memory writes (`stnl`), pointer arithmetic, nested loops, and stack operations.
- **`array_sum.asm`**, **`area.asm`**, **`add.asm`**, **`branch_show.asm`**: Verifies mathematical computations and control flow.

---

## Author & Academic Declaration

- **Author**: Anurag Nath
- **Roll Number**: 2301CS07
- **Institution**: Indian Institute of Technology Patna (IIT Patna)
- **Department**: Department of Computer Science & Engineering
- **Course**: CS2102 / CS321 / CS322 (Digital Logic and Computer Organization)
