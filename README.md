# CASM — A Custom Assembly Language, Assembler, and Virtual Machine

CASM (short for "C Assembly") is a small but complete toy computing stack built from
scratch in Python and C. It takes a hand-written `.casm` assembly source file all the
way down to raw machine bytecode and executes it on a register-based virtual CPU,
mirroring — at a miniature scale — the same pipeline a real toolchain uses:

```
source.casm  →  Lexer/Parser (Python)  →  JSON IR  →  Assembler (Python)  →  .bin bytecode  →  Virtual CPU (C)
```

The project was built to explore how assembly languages, instruction encoding, and
CPU execution actually work under the hood — without the safety net of an existing
ISA or framework.

---

## Table of Contents

- [Architecture Overview](#architecture-overview)
- [Project Structure](#project-structure)
- [Build & Run](#build--run)
- [The CASM Language](#the-casm-language)
- [Instruction Set Reference](#instruction-set-reference)
- [Bytecode Encoding](#bytecode-encoding)
- [Virtual Machine Internals](#virtual-machine-internals)
- [Memory Layout](#memory-layout)
- [The Stack](#the-stack)
- [Error Handling](#error-handling)
- [Example Walkthrough](#example-walkthrough)
- [Design Notes & Limitations](#design-notes--limitations)
- [Possible Extensions](#possible-extensions)

---

## Architecture Overview

The system is split into three independent stages, each of which can be reasoned
about (and tested) in isolation:

| Stage | Language | File | Responsibility |
|---|---|---|---|
| 1. Lexer / Parser | Python | `source/Language.py` | Converts raw `.casm` text into a structured JSON intermediate representation (a list of instruction tuples). |
| 2. Assembler | Python | `source/ByteCode.py` | Converts the JSON IR into raw binary opcodes/operands, writing a flat `.bin` file. |
| 3. Virtual CPU | C | `source/Cpu.c`, `source/Instructions.c`, `source/Tools.c` | Loads the binary into a simulated memory space and executes it instruction-by-instruction via a fetch-decode-execute loop. |

This separation means the **assembly syntax**, the **binary encoding**, and the
**execution semantics** are all decoupled — changing the instruction set only
requires updating the opcode table and the corresponding handler, not rewriting
the whole pipeline.

```
 ┌────────────┐      ┌──────────────┐      ┌──────────────┐      ┌─────────────┐
 │ test.casm  │ ───▶ │ Language.py  │ ───▶ │ ByteCode.py  │ ───▶ │ program.bin │
 │ (source)   │      │ (lex/parse)  │      │ (assemble)   │      │ (binary)    │
 └────────────┘      └──────────────┘      └──────────────┘      └──────┬──────┘
                                                                          │
                                                                          ▼
                                                                  ┌──────────────┐
                                                                  │   ./program  │
                                                                  │  (C VM/CPU)  │
                                                                  └──────────────┘
```

---

## Project Structure

```
.
├── README.md
├── run.sh                  # one-shot build + assemble + execute pipeline
├── test.casm                # sample CASM program exercising every instruction
├── bin/
│   └── program.bin           # assembled bytecode (generated)
├── source/
│   ├── Language.py           # lexer + parser → JSON IR
│   ├── ByteCode.py           # JSON IR → raw bytecode
│   ├── Cpu.c                 # fetch-decode-execute loop, opcode dispatch
│   ├── Instructions.c        # implementation of each instruction's semantics
│   └── Tools.c                # shared helpers (32-bit memory read/write)
└── include/
    ├── Instructions.h        # Register struct, CPU state, opcode prototypes
    ├── Tools.h                 # helper function prototypes
    └── Errors.h                 # shared error code enum
```

---

## Build & Run

The entire pipeline is driven by a single script:

```bash
./run.sh test.casm
```

`run.sh` performs the following steps automatically:

1. Recreates `./bin/program.bin` as an empty placeholder.
2. Compiles every `.c` file under `source/` with `gcc` into a `program` executable.
3. Pipes the `.casm` source through the Python lexer/parser (`Language.py`),
   producing JSON, which is piped directly into the assembler (`ByteCode.py`),
   producing raw bytecode written to `bin/program.bin`.
4. Executes the compiled VM against the freshly assembled binary:
   `./program ./bin/program.bin`.

**Requirements:** `gcc`, `python3` (no external Python packages required).

---

## The CASM Language

CASM is a line-oriented, whitespace-delimited assembly language. Each instruction
occupies its own line and is terminated with a semicolon `;` (everything after the
`;` on a line is also treated as a comment, including standalone comment lines):

```casm
; ---------- MOV tests ----------
MOV rax 10;
MOV rbx 5;

; ---------- ADD tests ----------
ADD rax rbx;
ADD rbx 7;

HLT;
```

Most arithmetic instructions accept **either** a second register **or** an
immediate integer literal as their second operand — the parser/assembler
determines which form to encode based on whether the token matches a known
register name.

### Available Registers

| Register | Purpose |
|---|---|
| `rax` | General purpose |
| `rbx` | General purpose |
| `rcx` | General purpose |
| `rdx` | General purpose |

---

## Instruction Set Reference

| Mnemonic | Operands | Effect | Opcode (Reg form) | Opcode (Imm form) |
|---|---|---|---|---|
| `MOV` | `reg, reg\|imm` | `reg1 = reg2` or `reg1 = imm` | `0x04` | `0x03` |
| `ADD` | `reg, reg\|imm` | `reg1 += reg2` or `reg1 += imm` | `0x06` | `0x05` |
| `MUL` | `reg, reg\|imm` | `reg1 *= reg2` or `reg1 *= imm` | `0x08` | `0x07` |
| `DIV` | `reg, reg\|imm` | `reg1 /= reg2` or `reg1 /= imm` | `0x0a` | `0x09` |
| `MOD` | `reg, reg\|imm` | `reg1 %= reg2` or `reg1 %= imm` | `0x0c` | `0x0b` |
| `MVN` | `reg` | Negates the register's value (treated as signed) | `0x0e` | — |
| `PUSH` | `imm` | Pushes a 32-bit immediate onto the stack | `0x02` | — |
| `POP` | `reg` | Pops the top of the stack into a register | `0x01` | — |
| `LOG` | `reg` | Prints `<name>: <value>` to stdout | `0x0d` | — |
| `HLT` | — | Halts CPU execution | `0xff` | — |

All arithmetic on register values is performed as **unsigned 32-bit** integers;
`MVN` is the one exception, explicitly reinterpreting the bit pattern as signed
to allow negation.

---

## Bytecode Encoding

The assembler (`ByteCode.py`) emits a flat stream of bytes with no headers, no
sections, and no symbol table — a deliberately minimal format. Each instruction
is encoded as:

```
[ opcode: 1 byte ] [ operand bytes... ]
```

| Operand type | Size | Encoding |
|---|---|---|
| Register | 1 byte | Register ID (`rax=0x00, rbx=0x01, rcx=0x02, rdx=0x03`) |
| Immediate | 4 bytes | Little-endian, signed 32-bit two's complement |

**Example:** `MOV rax 10;` is encoded as 6 bytes:

```
03            -- opcode MOV_IMM
00            -- register rax
0A 00 00 00   -- immediate value 10, little-endian
```

**Example:** `ADD rax rbx;` is encoded as 3 bytes:

```
06   -- opcode ADD_REG
00   -- register rax
01   -- register rbx
```

---

## Virtual Machine Internals

The VM (`Cpu.c`) implements a classic **fetch–decode–execute** loop over a flat
byte-addressable memory array:

```c
while (CPU.running) {
    uint8_t opcode = Memory[PC++];
    switch (opcode) {
        case 0x03: /* MOV_IMM */  ...
        case 0x04: /* MOV_REG */  ...
        ...
    }
}
```

- The **program counter (`PC`)** is a `uint16_t` indexing directly into the
  `Memory` array.
- `fetch_32()` (in `Tools.c`) reads a little-endian 32-bit value from memory and
  advances the given pointer by 4 — used both for fetching immediate operands
  during decode and for popping values off the stack.
- `getReg()` maps a raw register-ID byte from the bytecode stream to a pointer
  to the corresponding global `Register` struct (`rax`, `rbx`, `rcx`, `rdx`).
- Every instruction handler returns an `ErrorCodes` status, which `Cpu.c`
  inspects immediately after the call to decide whether to halt execution or
  print a diagnostic and continue.

---

## Memory Layout

The VM exposes a single 64KB (`0xFFFF`) address space, statically partitioned
into three regions:

```
0x0000 ───────────────────────────────────────────────► 0xFFFF
┌───────────────────────┬─────────────────┬──────────────────────┐
│   0x0000 – 0x7FFF      │  0x8000–0x9FFF  │    0xA000 – 0xFFFF    │
│   Program (code)        │  Data segment    │   Stack (grows down)  │
└───────────────────────┴─────────────────┴──────────────────────┘
```

| Region | Range | Purpose |
|---|---|---|
| Program | `0x0000`–`0x7FFF` | Loaded bytecode, executed by the fetch loop |
| Data | `0x8000`–`0x9FFF` | Reserved for future variable storage |
| Stack | `0xA000`–`0xFFFF` | `PUSH`/`POP` operate here |

This separation is a deliberate (if simplified) echo of how real architectures
isolate text, data, and stack segments to prevent code from accidentally
overwriting itself or colliding with runtime data.

---

## The Stack

CASM uses a **downward-growing stack**, the same convention used by most
real-world architectures (e.g., x86):

- `StackPointer` starts at `STACK_TOP` (`0xFFFF`), representing an empty stack.
- **`PUSH`** first decrements `StackPointer` by 4, then writes the 32-bit value
  at the new location — so the pointer always references the most recently
  pushed value.
- **`POP`** reads the 32-bit value currently at `StackPointer`, then advances
  the pointer forward by 4 (via `fetch_32`), restoring it to the previous frame.
- Pushing past `STACK_BOTTOM` (`0xA000`) raises `STACK_OVERFLOW`; popping an
  empty stack (`StackPointer == STACK_TOP`) raises `STACK_UNDERFLOW`.

---

## Error Handling

All instruction handlers in `Instructions.c` return a shared `ErrorCodes` enum
(`include/Errors.h`), letting the dispatch loop in `Cpu.c` react uniformly
regardless of which instruction triggered the fault:

| Error | Trigger | VM Behavior |
|---|---|---|
| `NULL_POINTER_EXCEPTION` | Invalid/unrecognized register ID passed to `getReg()` | Prints diagnostic, halts (`return 1`) |
| `DIVISION_BY_ZERO` | `DIV`/`MOD` with a zero divisor (immediate or register) | Prints diagnostic, **execution continues** |
| `STACK_OVERFLOW` | `PUSH` when `StackPointer <= STACK_BOTTOM` | Prints diagnostic, **execution continues** |
| `STACK_UNDERFLOW` | `POP` on an empty stack | Prints diagnostic, halts (`return 1`) |
| *(unknown opcode)* | Byte in the program region doesn't match any case | Prints opcode + PC, halts (`return 1`) |

---

## Example Walkthrough

`test.casm` exercises the full instruction set in sequence. A condensed trace:

```casm
MOV rax 10;   ; rax = 10
MOV rbx 5;    ; rbx = 5
ADD rax rbx;  ; rax = 15
MUL rcx 2;    ; rcx = 40   (rcx was set to 20 earlier)
DIV rax rbx;  ; rax = 3    (15 / 5)
MOD rax 3;    ; rax = 0    (3 % 3)
MVN rax;      ; rax = 0 negated = 0
PUSH 100;
PUSH 200;
POP rax;      ; rax = 200
POP rbx;      ; rbx = 100
LOG rax;      ; prints "rax: 200"
HLT;
```

Running `./run.sh test.casm` assembles and executes this in one shot, printing
the result of each `LOG` instruction to stdout.

---

## Design Notes & Limitations

- **No labels or control flow (yet):** there is no `JMP`, no conditional
  branching, and no loop construct — every program is a straight-line sequence
  of instructions ending in `HLT`. This was an intentional scoping decision to
  first get a clean fetch–decode–execute pipeline and instruction encoding
  scheme working end-to-end before introducing control flow and the
  jump-target resolution it requires.
- **Fixed-width register set:** four general-purpose registers, no
  general addressing modes beyond direct register/immediate operands.
- **No bounds checking on JSON→bytecode stage:** malformed `.casm` input
  (e.g., referencing an undefined register) will raise a Python `KeyError`
  rather than a friendly assembler error — acceptable for a learning project,
  but a clear next step for hardening.
- **Single source-of-truth opcode table**, duplicated across `ByteCode.py`'s
  `OPCODES` dict and `Cpu.c`'s `switch` statement — kept in sync manually.

## Possible Extensions

- Conditional jumps (`JMP`, `JE`, `JNE`, `JG`, `JL`) and label resolution in the assembler
- A `CALL`/`RET` mechanism for subroutines, built on top of the existing stack
- Memory-addressed load/store instructions targeting the data segment (`0x8000–0x9FFF`)
- A disassembler to round-trip `.bin` files back into readable mnemonics
- Assembler-level syntax errors with line numbers, instead of raw Python exceptions