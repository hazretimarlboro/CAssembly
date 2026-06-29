import json
import sys

def read_file(filename:str):
    with open(filename,"r") as file:
        content = file.read()
        return content

def parse_program(text:str):
    instructions = []

    for line in text.split(";"):
        line = line.strip()
        if not line:
            continue

        parts = line.split()
        op = parts[0]

        args = parts[1:]

        instructions.append((op, args))

    return instructions

with open("./json/output.json", "w") as file:
    json.dump(parse_program(read_file(sys.argv[1])), file)
