uint8_t CPU_STATE = 0x01;
uint8_t STATUS_REGISTER = 0x00;

bool CLOCK = 0;
uint16_t PROGRAM_COUNTER = 0x8000;
uint16_t TEMP_REGISTER = 0x0000;
uint8_t STACK_POINTER = 0x00;
uint8_t REGISTER_A = 0x0a;
uint8_t REGISTER_B = 0x00;
bool block_pincrement = false;

bool irq_pin = false;
uint8_t irq_index = 0;

uint16_t ADDRESS_BUS = 0x0000;
uint8_t DATA_BUS = 0x00;

#include "cpu_decoder.h"
#include "cpu_alu.h"
#include "io.h"


bool check_decoded(uint8_t index) {
    return decoded_instruction & (0x01 << index);
}

void debug(uint8_t type) {
    switch (type) {
        case 0: {
            printf("OP: %02x\n", decoder_opcode);
            printf("PC: %02x, TR: %04x, SP: %02x\n", PROGRAM_COUNTER, TEMP_REGISTER, STACK_POINTER);
            printf("DATA: %02x, ADDR: %04x\n", DATA_BUS, ADDRESS_BUS);
            printf("A: %02x, B: %02x\n", REGISTER_A, REGISTER_B);
            printf("CS: %02x, SR: %02x\n", CPU_STATE, STATUS_REGISTER);
            break;    
        }
        case 1: {
            printf("PC: %02x, CPU_STATE: %02x, OPCODE: %02x, MICROINTRUCTION: %02x\n", PROGRAM_COUNTER, CPU_STATE, decoder_opcode, decoded_instruction);
            break;
        }
        case 2: {
            printf("A: %02x, STATUS: %02x\n", REGISTER_A, STATUS_REGISTER);
            break;
        }
        case 3: {
            if (STATUS_REGISTER & 0x80) {printf("halted\n");}
        }
    }
}

void irq_update() {
    if (irq_pin & irq_index == 0) {
        irq_index = 1;
        printf("Interrupt Initialized\n");
    } else if (!irq_pin & irq_index == 0) {
        return;
    }

    if (CPU_STATE == 0) {
        irq_index++;
        printf("Inrement\n");

        if (irq_index == 7) {
            irq_index = 0;
            printf("Interrupt End\n");
            printf("New PC: %04x\n", PROGRAM_COUNTER);
        }
    }

    switch (irq_index) {
        case 2: {DATA_BUS = 0x3e; break;}
        case 3: {DATA_BUS = 0x3f; break;}
        case 4: {DATA_BUS = 0xff; break;}
        case 5: {DATA_BUS = 0xfd; break;}
        case 6: {DATA_BUS = 0x22; break;}
    }
}

void cpu_update() {

    if (STATUS_REGISTER & 0b10000000) {return;}

    // I kind of wish I had another way to do this that didn't involve using a billion if statements, but
    // to keep it simple and as accurate as posssible (that I think I can make) sacrifices had to be made :(

    if (CPU_STATE == 0) {
        if (!block_pincrement) {PROGRAM_COUNTER++;}
        block_pincrement = false;
        CPU_STATE++;
        ALU_MASK = 0;
        return;
    }
    else if (CPU_STATE == 1) {
        ADDRESS_BUS = PROGRAM_COUNTER;
        DATA_BUS = io_read(ADDRESS_BUS);
        //irq_update();
        decoder_opcode = DATA_BUS;
        CPU_STATE++;
        return;
    }

    
    if (check_decoded(9)) {++PROGRAM_COUNTER;}
    if (check_decoded(7)) {ADDRESS_BUS = PROGRAM_COUNTER;}
    if (check_decoded(4)) {ADDRESS_BUS = TEMP_REGISTER;}
    if (check_decoded(12)) {ADDRESS_BUS = (uint16_t)STACK_POINTER;}

    if (check_decoded(13)) {DATA_BUS = io_read(ADDRESS_BUS);}
    if (check_decoded(0)) {DATA_BUS = REGISTER_A;}
    if (check_decoded(1)) {DATA_BUS = REGISTER_B;}
    if (check_decoded(24)) {DATA_BUS = STATUS_REGISTER;}
    if (check_decoded(26)) {DATA_BUS = alu_register;}
    if (check_decoded(29)) {DATA_BUS = (uint8_t)((TEMP_REGISTER & 0xFF00) >> 8);}
    if (check_decoded(30)) {DATA_BUS = (uint8_t)(TEMP_REGISTER & 0x00FF);}


    //if (check_decoded(0x1B)) {printf("Zero Read: %02x\n", STATUS_REGISTER);}
    //if (check_decoded(0x1C)) {printf("Carry Read: %02x\n", STATUS_REGISTER);}

    //irq_update();

    if (check_decoded(2)) {REGISTER_A = DATA_BUS;}
    if (check_decoded(3)) {REGISTER_B = DATA_BUS;}
    if (check_decoded(5)) {
        TEMP_REGISTER &= 0x00FF;
        TEMP_REGISTER |= (DATA_BUS << 8);
    }
    if (check_decoded(6)) {
        TEMP_REGISTER &= 0xFF00;
        TEMP_REGISTER |= DATA_BUS;
    }

    if (check_decoded(8)) {PROGRAM_COUNTER = ADDRESS_BUS; block_pincrement = true;}

    if (check_decoded(14)) {io_write(ADDRESS_BUS, DATA_BUS);}
    if (check_decoded(10)) {STACK_POINTER++;}
    if (check_decoded(11)) {STACK_POINTER--;}
    if (check_decoded(15)) {ALU_MASK = 0b00000001;}
    if (check_decoded(16)) {ALU_MASK = 0b00000010;}
    if (check_decoded(17)) {ALU_MASK = 0b00000100;}
    if (check_decoded(18)) {ALU_MASK = 0b00001000;}
    if (check_decoded(19)) {ALU_MASK = 0b00010000;}
    if (check_decoded(20)) {ALU_MASK = 0b00100000;}
    if (check_decoded(21)) {ALU_MASK = 0b01000000;}
    if (check_decoded(22)) {ALU_MASK = 0b10000000;}
    if (check_decoded(23)) {TEMP_REGISTER = ADDRESS_BUS;}
    if (check_decoded(25)) {STATUS_REGISTER = DATA_BUS;}
    if (check_decoded(31)) {CPU_STATE = 0; return;}
    //if (check_decoded(27) & (STATUS_REGISTER & 0b1)) {CPU_STATE = 0; printf("EZ Called\n"); return;}
    //if (check_decoded(28) & (STATUS_REGISTER & 0b01)) {CPU_STATE = 0; printf("ECA Called\n"); return;}

    if (ALU_MASK != 0) {
        //printf("Hi\n");
        alu_update();
        uint8_t arithmetic_flags = (carry_out << 1) | (zero_out);
        STATUS_REGISTER &= 0b11111100;
        STATUS_REGISTER |= arithmetic_flags;
        ALU_MASK = 0;
    }
    //debug();
}