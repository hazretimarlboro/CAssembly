# CASM — A Custom Assembly Language & Virtual Machine

CASM is a small, complete toolchain: a hand-rolled assembly-like language, an
assembler, and a register-based virtual CPU that executes the resulting
bytecode. It was built in two languages on purpose — Python for the
front end (lexing, parsing, encoding) and C for the back end (the CPU
itself) — to mirror how real toolchains like GCC or the JVM separate
"understanding the source" from "executing the machine."

```
 your .casm file
        │
        ▼
 ┌─────────────┐        ┌──────────────┐        ┌───────────────┐
 │ Language.py │──JSON─▶│ ByteCode.py  │──.bin─▶│   C Runtime   │
 │ lexer/parser│        │  assembler   │        │  (the "CPU")  │
 └─────────────┘        └──────────────┘        └───────────────┘
   tokens → AST         opcodes + operands         fetch → decode →
                          → raw bytes                execute loop
```

Each stage is a separate, inspectable process — you can literally run
`python3 source/Language.py program.casm` on its own and see the parsed
instruction list as JSON before it ever becomes bytecode.

---

## Quick start

```bash
./run.sh examples/demo.casm
```

This assembles `examples/demo.casm`, links the C runtime, and executes the
resulting bytecode. The demo program computes `5!` (factorial) using a
countdown loop, round-trips a value through the stack, and demonstrates
signed values with `MVN`:

```
rax: 120
rax: 0
rax: 120
rax: -120
```

---

## Language reference

Four general-purpose registers, no floating point, no memory-mapped I/O —
just enough to be Turing-complete and fun to program in.

| Register | Purpose            |
|----------|---------------------|
| `rax`    | general purpose     |
| `rbx`    | general purpose     |
| `rcx`    | general purpose     |
| `rdx`    | general purpose     |

### Instruction set

| Instruction | Form | Effect |
|---|---|---|
| `MOV`  | `MOV reg, reg\|imm`  | Copy a value into a register |
| `ADD`  | `ADD reg, reg\|imm`  | Add into a register |
| `MUL`  | `MUL reg, reg\|imm`  | Multiply into a register |
| `DIV`  | `DIV reg, reg\|imm`  | Integer divide into a register |
| `MOD`  | `MOD reg, reg\|imm`  | Modulo into a register |
| `MVN`  | `MVN reg`            | Negate a register (two's complement) |
| `CMP`  | `CMP reg, reg\|imm`  | Set the comparison flag (equality) |
| `JMP`  | `JMP label`          | Unconditional jump |
| `JEQ`  | `JEQ label`          | Jump if the last `CMP` was equal |
| `JNE`  | `JNE label`          | Jump if the last `CMP` was not equal |
| `PUSH` | `PUSH imm`           | Push an immediate onto the stack |
| `POP`  | `POP reg`            | Pop the stack into a register |
| `LOG`  | `LOG reg`            | Print a register's value (signed decimal) |
| `HLT`  | `HLT`                | Stop execution |

Every arithmetic and comparison instruction has both an **immediate** form
(`ADD rax, 5`) and a **register** form (`ADD rax, rbx`) — the assembler picks
the right opcode automatically based on whether the second operand is a
register name.

Comments start with `;` and run to the end of the line. Labels are any
identifier ending in `:`, resolved to absolute program addresses in a single
pre-pass before code generation, so forward references (jumping to a label
defined later in the file) just work.

### Memory layout

```
0x0000 – 0x7FFF   program (max 32,767 bytes)
0x8000 – 0x9FFF   reserved for data
0xA000 – 0xFFFF   stack (grows downward from 0xFFFF)
```

The stack grows *downward*: `PUSH` decrements the stack pointer before
writing, `POP` reads then increments it back. This is the same convention
x86 uses, chosen so the stack can grow toward the program/data region
without colliding with a separately-growing heap.

---

## How execution actually works

The C runtime is a classic **fetch–decode–execute loop**: read one opcode
byte at `Memory[PC]`, advance `PC`, `switch` on the opcode to pull in
whatever operand bytes that instruction needs, run it, repeat until `HLT`
or an unrecoverable error.

Every instruction handler returns a status code (`SUCCESS`,
`DIVISION_BY_ZERO`, `STACK_UNDERFLOW`, `NULL_POINTER_EXCEPTION`, ...) rather
than crashing outright — invalid register bytes, division by zero, and
stack exhaustion are all caught and reported with a specific error message
and a non-zero exit code instead of segfaulting.

---

### Known limitation (by design, not a bug)

`PUSH` only accepts an immediate value (`PUSH 42`), not a register
(`PUSH rax` is a parse-time error). Extending it to support pushing a
register's live value would need a second opcode (`PUSH_REG`, mirroring how
every other instruction has `_IMM`/`_REG` variants) — a natural next step
if the project continues.

---

## Project structure

```
.
├── run.sh                  # build + assemble + run, one command
├── source/
│   ├── Language.py         # lexer + parser → JSON AST
│   ├── ByteCode.py         # AST → raw bytecode
│   ├── Cpu.c                # fetch-decode-execute loop, entry point
│   ├── Instruction.c        # instruction implementations
│   └── Tools.c              # little-endian read/write helpers
├── include/
│   ├── Instructions.h       # register/CPU state, memory layout constants
│   ├── Tools.h
│   └── Errors.h             # shared error codes
└── examples/
    └── demo.casm            # factorial + stack + MVN demo
```

## Design decisions worth calling out

- **Two languages, one pipeline.** Python's expressiveness makes lexing and
  parsing pleasant; C's control over memory layout makes a believable "CPU."
  Keeping them as separate processes connected by a well-defined bytecode
  format (rather than, say, embedding Python in C) keeps each half simple
  and independently testable.
- **Every instruction handler returns a status code.** No handler trusts
  its inputs — invalid registers, division by zero, and stack bounds are
  all checked explicitly rather than left to undefined behavior.
- **Fixed-width types throughout** (`uint8_t`, `uint16_t`, `uint32_t`)
  instead of `int`/`long`, so the memory layout and instruction encoding
  are exact and portable rather than dependent on platform integer sizes.
