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

# ---------- PARSER ----------
def parse(tokens):
    instructions = []

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
        
    
        else:
            raise Exception(f"Unknown instruction: {op}")

    return instructions


# ---------- DRIVER ----------
def assemble(file_path):
    with open(file_path, "r") as f:
        text = f.read()

    tokens = lex(text)
    program = parse(tokens)

    return program


# ---------- TEST ----------
if __name__ == "__main__":
    program = assemble(sys.argv[1])

    print(json.dumps(program))