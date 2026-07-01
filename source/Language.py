import sys
import json
# ---------- LEXER ----------
def lex(program_text):
    tokens = []

    for line in program_text.splitlines():
        # remove anything after ';' (comment behavior)
        line = line.split(";")[0]

        line = line.strip()

        # skip empty lines
        if not line:
            continue

        parts = line.split()
        tokens.append(parts)

    return tokens

def is_register(x):
    return x in ["rax","rbx","rcx","rdx"]

# ---------- Instruction Sizes ----------
def instr_size(parts):
    op = parts[0]

    if op.endswith(":"):
        return 0

    if op == "HLT":
        return 1

    if op == "LOG":
        return 2

    if op == "POP":
        return 2

    if op == "MVN":
        return 2

    if op == "PUSH":
        return 5
    
    if op == "JMP":
        return 5

    if op == "JNE":
        return 5
    
    if op == "JEQ":
        return 5

    if op in ("MOV", "ADD", "MUL", "DIV", "MOD", "CMP"):
        if is_register(parts[2]):
            return 3
        else:
            return 6
    


    return 0


# ---------- PARSER ----------
def parse(tokens):
    instructions = []
    labels = {}
    pc = 0x0000

    for parts in tokens:
        if parts[0].endswith(":"):
            labels[parts[0][:-1]] = pc
        else:
            pc += instr_size(parts)

    for parts in tokens:
        op = parts[0]

        if op == "ADD":
            instructions.append(("ADD", parts[1], parts[2]))

        elif op == "PUSH":
            instructions.append(("PUSH", parts[1]))

        elif op == "POP":
            instructions.append(("POP", parts[1]))

        elif op == "MOV":
            instructions.append(("MOV", parts[1], parts[2]))

        elif op == "MUL":
            instructions.append(("MUL", parts[1], parts[2]))

        elif op == "DIV":
            instructions.append(("DIV", parts[1], parts[2]))

        elif op == "MOD":
            instructions.append(("MOD", parts[1], parts[2]))

        elif op == "MVN":
            instructions.append(("MVN",parts[1]))

        elif op == "LOG":
            instructions.append(("LOG", parts[1]))

        elif op == "HLT":
            instructions.append(("HLT",))
        
        elif str(op).endswith(":"):
            continue
        
        elif op == "JMP":
            instructions.append(("JMP", parts[1]))

        elif op == "JNE":
            instructions.append(("JNE",parts[1]))

        elif op == "JEQ":
            instructions.append(("JEQ",parts[1]))

        elif op == "CMP":
            instructions.append(("CMP", parts[1], parts[2]))

        else:
            raise Exception(f"Unknown instruction: {op}")

    return instructions,labels


# ---------- DRIVER ----------
def assemble(file_path):
    with open(file_path, "r") as f:
        text = f.read()

    tokens = lex(text)
    program,labels = parse(tokens)

    return program,labels


# ---------- TEST ----------
if __name__ == "__main__":
    program = assemble(sys.argv[1])

    print(json.dumps(program))
