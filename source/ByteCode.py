import sys
import json

OPCODES = {
    "POP":      0x01,
    "PUSH":     0x02,
    "MOV_IMM":  0x03,
    "MOV_REG":  0x04,
    "ADD_IMM":  0x05,
    "ADD_REG":  0x06,
    "MUL_IMM":  0x07,
    "MUL_REG":  0x08,
    "DIV_IMM":  0x09,
    "DIV_REG":  0x0a,
    "MOD_IMM":  0x0b,
    "MOD_REG":  0x0c,
    "LOG"    :  0x0d,
    "MVN"    :  0x0e,
    "HLT"    :  0xff
}

REGISTERS = {
    "rax": 0x00,
    "rbx": 0x01,
    "rcx": 0x02,
    "rdx": 0x03
}

def toByte(number: int):
    return number.to_bytes(4, "little", signed=True)

data = json.loads(sys.stdin.read())

bytecode = []

for inst in data:
    op = inst[0]

    if op == "POP":
        reg = REGISTERS[inst[1]]
        bytecode.append(OPCODES["POP"])
        bytecode.append(reg)

    elif op == "MOV":
        reg1 = REGISTERS[inst[1]]
        if inst[2] in REGISTERS:
            #then this is a register moving operation
            reg2 = REGISTERS[inst[2]]
            bytecode.append(OPCODES["MOV_REG"])
            bytecode.append(reg1)
            bytecode.append(reg2)
        else:
            #then this is a immediate value moving operation
            imm = int(inst[2])
            bytecode.append(OPCODES["MOV_IMM"])
            bytecode.append(reg1)
            bytecode.extend(toByte(imm))

    elif op == "ADD":
        reg1 = REGISTERS[inst[1]]
        if inst[2] in REGISTERS:
            reg2 = REGISTERS[inst[2]]
            bytecode.append(OPCODES["ADD_REG"])
            bytecode.append(reg1)
            bytecode.append(reg2)
        else:
            imm = int(inst[2])
            bytecode.append(OPCODES["ADD_IMM"])
            bytecode.append(reg1)
            bytecode.extend(toByte(imm))

    elif op == "MUL":
        reg1 = REGISTERS[inst[1]]
        if inst[2] in REGISTERS:
            reg2 = REGISTERS[inst[2]]
            bytecode.append(OPCODES["MUL_REG"])
            bytecode.append(reg1)
            bytecode.append(reg2)
        else:
            imm = int(inst[2])
            bytecode.append(OPCODES["MUL_IMM"])
            bytecode.append(reg1)
            bytecode.extend(toByte(imm))

    elif op == "DIV":
        reg1 = REGISTERS[inst[1]]
        if inst[2] in REGISTERS:
            reg2 = REGISTERS[inst[2]]
            bytecode.append(OPCODES["DIV_REG"])
            bytecode.append(reg1)
            bytecode.append(reg2)
        else:
            imm = int(inst[2])
            bytecode.append(OPCODES["DIV_IMM"])
            bytecode.append(reg1)
            bytecode.extend(toByte(imm))

    elif op == "MOD":
        reg1 = REGISTERS[inst[1]]
        if inst[2] in REGISTERS:
            reg2 = REGISTERS[inst[2]]
            bytecode.append(OPCODES["MOD_REG"])
            bytecode.append(reg1)
            bytecode.append(reg2)
        else:
            imm = int(inst[2])
            bytecode.append(OPCODES["MOD_IMM"])
            bytecode.append(reg1)
            bytecode.extend(toByte(imm))

    elif op == "MVN":
        reg1 = REGISTERS[inst[1]]
        bytecode.append(OPCODES["MVN"])
        bytecode.append(reg1)

    elif op == "PUSH":
        imm = int(inst[1])
        bytecode.append(OPCODES["PUSH"])
        bytecode.extend(toByte(imm))

    elif op == "LOG":
        reg1 = REGISTERS[inst[1]]
        bytecode.append(OPCODES["LOG"])
        bytecode.append(reg1)

    elif op == "HLT":
        bytecode.append(OPCODES["HLT"])




assert all(0 <= b <= 255 for b in bytecode), "Bytecode out of range!"

sys.stdout.buffer.write(bytearray(bytecode))