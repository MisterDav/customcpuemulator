import sys
import os
import conv_bin

opcodes = {
    "mova": 0,
    "movb": 1,
    "lda": 2,
    "ldb": 3,
    "ldai": 4,
    "ldbi": 5,
    "sta": 6,
    "stb": 7,
    "pua": 8,
    "poa": 9,
    "tss": 10,
    "wsr": 11,
    "nop": 12,
    "add": 13,
    "adc": 14,
    "addi": 15,
    "sub": 17,
    "sbc": 18,
    "subi": 19,
    "or": 21,
    "ori": 22,
    "xor": 23,
    "xori": 24,
    "and": 25,
    "andi": 26,
    "shr": 27,
    "shl": 28,
    "cmp": 29,
    "cmpi": 30,
    "not": 31,
    "je": 32,
    "jc": 33,
    "jmp": 34,
    "call": [35, 34],
    "ret": 37,
    "rti": [38, 39],
    "pub": 40,
    "pob": 41,
    "ldbo": 42,
    "stbo": 42
}
imm8_opcodes = ["ldai", "ldbi", "addi", "subi", "ori", "xori", "andi", "shr", "shl", "cmpi"]
imm16_opcodes = ["lda", "sta", "ldb", "stb", "je", "jc", "jmp", "call", "ldbo", "stbo"]
jump_opcodes = ["je", "jc", "jmp", "call"]

def convert_number(number:str):
    if number[0] == "$":
        return int(number[1::])
    elif number[0] == "#":
        return int(number[1::], 16)
    elif number[0] == "%":
        return int(number[1::], 2)
    else:
        return number                               # If cannot be converted into a number, then it's likely a label

bytecode = []   
labels = {}
constants = {}
raw = open(sys.argv[1], "r").readlines()

for lines in raw:
    if lines == "\n": continue
    arguments = list(filter(None, lines.replace("\n", "").split(" ")))

    if arguments[0] in opcodes.keys():
        # Retrieves the opcode from the dictionary
        opcode_decoded = opcodes[arguments[0]]
        if type(opcode_decoded) == int:             # Check if it's an integer
            bytecode.append(opcode_decoded)         # And add as-is
        elif type(opcode_decoded) == list:          # Or if it's a list
            for x in opcode_decoded:                # And add every item from that list
                bytecode.append(x)

        if arguments[0] in imm8_opcodes: 

            if arguments[1] in constants.keys():
                arguments[1] = constants[arguments[1]]

            if len(arguments) < 2:
                exit(f"Must have imm8 arguments for {arguments[0]} instruction.")

            full = convert_number(arguments[1])     # Convert the operand to an imm8
            if type(full) == str:
                exit(f"Labels ({full}) can only be used in place of imm16")
            if full < 0 or full > 0xFF:            # Make sure the value is within range
                exit(f"{arguments[0]} imm8 - imm8 {arguments[1]} cannot be less than zero or greater than 0xFF")
            bytecode.append(full & 0xFF)            # Then add it

        elif arguments[0] in imm16_opcodes:
            print(arguments)
            if arguments[1] in constants.keys():
                arguments[1] = constants[arguments[1]]

            full = convert_number(arguments[1])     # Convert the operand to an imm16

            if type(full) == int:
                if full < 0 or full > 0xFFFF:          # Make sure the value is within range
                    exit(f"{arguments[0]} imm16 - imm16 {arguments[1]} cannot be less than zero or greater than 0xFFFF")

                bytecode.append(( (full) & 0xFF00) >> 8)   # Then add them
                bytecode.append( (full) & 0x00FF)
                print(hex(full))
            
            elif type(full) == str:
                bytecode.append(full)                   # Save the label for later.
                bytecode.append(0x00)                   # And also some blank data.

            # In the special case that we use the offset instructions, then append their actual opcode after priming the data.
            if arguments[0] == "ldbo": bytecode.append(43)
            elif arguments[0] == "stbo": bytecode.append(44)

    elif arguments[0] == "label": 
        labels[arguments[1]] = 0x8000 + len(bytecode)
    elif arguments[0] == "org": 

        full = convert_number(arguments[1])

        # Check if the number is in range.
        if full < 0 or full > 0xFFFF: exit(f"ORG location ({hex(full)}) must be within 0x8000 - 0xFFFF")

        # Checks the amount of zeroes to add.
        amount_to_add = full - len(bytecode) - 0x8000

        # Makes sure no existing data is going to be overwritten
        if amount_to_add < 0: exit(f"ORG location ({hex(full)}) cannot overwrite any existing data")

        # Then adds the zeroes.
        for x in range(amount_to_add): bytecode.append(0x00)
    elif arguments[0] == "byte":
        for x in arguments[1::]:
            number = convert_number(x)
            if number < 0 or number > 0xFF:
                exit(f"Bytes ({hex(number)}) sequence must be between 0x00 - 0xFF")
            
            bytecode.append(number)
    elif arguments[0] == "long":
        for x in arguments[1::]:
            number = convert_number(x)
            if number < 0 or number > 0xFFFF:
                exit(f"Bytes ({hex(number)}) sequence must be between 0x00 - 0xFFFF")
            
            bytecode.append((number & 0xFF00) >> 8)
            bytecode.append(number & 0x00FF)
    elif arguments[0] == "equ":
        constants[arguments[1]] = arguments[2]
    else:
        exit(f"Unknown token '{arguments[0]}'")

for x in range(len(bytecode)):

    if type(bytecode[x]) == str and bytecode[x] in labels.keys():
        number = labels[bytecode[x]]
        bytecode[x] = (number & 0xFF00) >> 8
        bytecode[x+1] = number & 0xFF

    elif type(bytecode[x]) == str and not bytecode[x] in labels.keys():
        print(labels)
        exit(f"Unknown label {bytecode[x]}")

remaining_bytes = 0x8000 - len(bytecode)
if remaining_bytes < 0:
    exit("File size exceeds 32kb limit. Try optimizing your code! :nerd_emoji:")
for x in range(remaining_bytes):
    bytecode.append(0)

open("main.bin", "wb").write(bytearray(bytecode))
conv_bin.bin2logisim("main.bin", "out.bin")