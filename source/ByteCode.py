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
    "JMP"    :  0x0f,
    "JEQ"    :  0x10,
    "JNE"    :  0x11,
    "CMP_IMM":  0x12,
    "CMP_REG":  0x13,
    "PUSH_REG": 0x14,
    "CALL"   : 0x15,
    "RET"    : 0x16,
    "JL"     : 0x17,
    "JG"     : 0x18,
    "JLE"    : 0x19,
    "JGE"    : 0x1a,
    "LOAD_REG"   : 0x1b,
    "STORE_PTR_REG": 0x1c,
    "SUB_IMM"          : 0x1d,
    "SUB_REG"          : 0x1e,
    "HLT"    :  0xff
}

REGISTERS = {
    "rax": 0x00,
    "rbx": 0x01,
    "rcx": 0x02,
    "rdx": 0x03,
    "call": 0x04,
    "stack": 0x05
}

def toByte(number: int):
    return number.to_bytes(4, "little", signed=True)

data = json.loads(sys.stdin.read())
instructions = data[0]
labels = data[1]

bytecode = []

for inst in instructions:
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

    elif op == "SUB":
            reg1 = REGISTERS[inst[1]]
            if inst[2] in REGISTERS:
                reg2 = REGISTERS[inst[2]]
                bytecode.append(OPCODES["SUB_REG"])
                bytecode.append(reg1)
                bytecode.append(reg2)
            else:
                imm = int(inst[2])
                bytecode.append(OPCODES["SUB_IMM"])
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
        if inst[1] in REGISTERS:
            reg1 = REGISTERS[inst[1]]
            bytecode.append(OPCODES["PUSH_REG"])
            bytecode.append(reg1)
        else:
            imm = int(inst[1])
            bytecode.append(OPCODES["PUSH"])
            bytecode.extend(toByte(imm))

    elif op == "LOG":
        reg1 = REGISTERS[inst[1]]
        bytecode.append(OPCODES["LOG"])
        bytecode.append(reg1)

    elif op == "HLT":
        bytecode.append(OPCODES["HLT"])

    elif op == "RET":
        bytecode.append(OPCODES["RET"])

    elif op == "JMP":
        if inst[1] in labels:
            target = labels[inst[1]]
            bytecode.append(OPCODES["JMP"])
            bytecode.extend(toByte(target))
        else:
            raise Exception(f"[ByteCode Error] Unknown JMP label: {inst[1]}")
        
    elif op == "JNE":
        if inst[1] in labels:
            target = labels[inst[1]]
            bytecode.append(OPCODES["JNE"])
            bytecode.extend(toByte(target))
        else:
            raise Exception(f"[ByteCode Error] Unknown JNE label: {inst[1]}")
    
    elif op == "JEQ":
        if inst[1] in labels:
            target = labels[inst[1]]
            bytecode.append(OPCODES["JEQ"])
            bytecode.extend(toByte(target))
        else:
            raise Exception(f"[ByteCode Error] Unknown JEQ label: {inst[1]}")

    elif op == "JG":
        if inst[1] in labels:
            target = labels[inst[1]]
            bytecode.append(OPCODES["JG"])
            bytecode.extend(toByte(target))
        else:
            raise Exception(f"[ByteCode Error] Unknown JG label: {inst[1]}")

    elif op == "JL":
        if inst[1] in labels:
            target = labels[inst[1]]
            bytecode.append(OPCODES["JL"])
            bytecode.extend(toByte(target))
        else:
            raise Exception(f"[ByteCode Error] Unknown JL label: {inst[1]}")

    elif op == "JGE":
            if inst[1] in labels:
                target = labels[inst[1]]
                bytecode.append(OPCODES["JGE"])
                bytecode.extend(toByte(target))
            else:
                raise Exception(f"[ByteCode Error] Unknown JGE label: {inst[1]}")

    elif op == "JLE":
            if inst[1] in labels:
                target = labels[inst[1]]
                bytecode.append(OPCODES["JLE"])
                bytecode.extend(toByte(target))
            else:
                raise Exception(f"[ByteCode Error] Unknown JLE label: {inst[1]}")

    elif op == "CALL":
        if inst[1] in labels:
            target = labels[inst[1]]
            bytecode.append(OPCODES["CALL"])
            bytecode.extend(toByte(target))
        else:
            raise Exception(f"[ByteCode Error] Unknown CALL target: {inst[1]}")
        
    elif op == "CMP":
        reg1 = REGISTERS[inst[1]]
        if inst[2] in REGISTERS:
            reg2 = REGISTERS[inst[2]]
            bytecode.append(OPCODES["CMP_REG"])
            bytecode.append(reg1)
            bytecode.append(reg2)
        else:
            imm = int(inst[2])
            bytecode.append(OPCODES["CMP_IMM"])
            bytecode.append(reg1)
            bytecode.extend(toByte(imm))

    elif op == "LOAD":
        if inst[1] in REGISTERS and (not inst[2] in REGISTERS):
            # LOAD reg ptr
            value = int(inst[2],0)
            bytecode.append(OPCODES["LOAD_REG"])
            bytecode.append(REGISTERS[inst[1]])
            bytecode.extend(toByte(value))
        else:
            raise Exception("[ByteCode Error] LOAD instruction should be used like: LOAD <Register> <Address>")
            

    elif op == "STORE":
        if inst[1] in REGISTERS:
            raise Exception("[ByteCode Error] STORE instruction can not be used with a register as the first operand")
        else:
            if inst[2] in REGISTERS:
                reg = REGISTERS[inst[2]]
                lptr = int(inst[1], 0)
                bytecode.append(OPCODES["STORE_PTR_REG"])
                bytecode.extend(toByte(lptr))
                bytecode.append(reg)
            else:
                raise Exception("ByteCode Error STORE instruction should be used like: STORE <Address> <Register>")

for i, b in enumerate(bytecode):
    if not isinstance(b, int):
        raise TypeError(f"bytecode[{i}] = {b!r} ({type(b).__name__})")
    if not (0 <= b <= 255):
        raise ValueError(f"bytecode[{i}] = {b} is out of byte range")

sys.stdout.buffer.write(bytearray(bytecode))
