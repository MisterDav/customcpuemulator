uint8_t ALU_MASK = 0x00;

bool CARRY = false;
bool CARRY_ENABLED = false;

uint8_t alu_register = 0x00;
bool carry_out = false;
bool zero_out = false;

void alu_update() {

    if (ALU_MASK & 0b00000001) {                                                                // EA
        uint16_t result = REGISTER_A + DATA_BUS;
        alu_register = result & 0xFF;
        carry_out = (bool)(result & 0xFF00);
    }
    else if (ALU_MASK & 0b00000010) {                                                           // ES
        uint16_t result = DATA_BUS - REGISTER_A;
        alu_register = result & 0xFF;
        carry_out = (bool)(result & 0xFF00);
    }
    else if (ALU_MASK & 0b00000100) {alu_register = REGISTER_A | DATA_BUS;}                   // EO
    else if (ALU_MASK & 0b00001000) {alu_register = REGISTER_A & DATA_BUS;}                   // EAN
    else if (ALU_MASK & 0b00010000) {alu_register = REGISTER_A ^ DATA_BUS;}                   // EX
    else if (ALU_MASK & 0b00100000) {alu_register = REGISTER_A << (DATA_BUS & 0b00000111);}   // ESL
    else if (ALU_MASK & 0b01000000) {alu_register = REGISTER_A >> (DATA_BUS & 0b00000111);}   // ESR
    else if (ALU_MASK & 0b10000000) {alu_register = ~REGISTER_A;}                             // EN
    zero_out = alu_register & 0xFF == 0x00;
    //if (decoder_opcode == 0x20) {printf("ZERO OUT: %u\n", zero_out);}
    //if (ALU_MASK != 0) {printf("RA: %02x, Data Bus: %02x\n", REGISTER_A, DATA_BUS);}
}