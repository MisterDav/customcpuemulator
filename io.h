uint8_t rom_contents[0x8000] = {0x04, 0x80, 0x0b, 0x22, 0x00, 0x00};
uint8_t ram_contents[0x8000] = {0x00};

void ram_dump() {
    FILE *file;
    file = fopen("ram.bin", "w");
    fwrite(ram_contents, sizeof(uint8_t), 0x8000, file);
    fclose(file);
}

void load_rom(char *filepath) {
    FILE *file_pointer;
    file_pointer = fopen(filepath, "r");
    fgets(rom_contents, 0x7FFF, file_pointer);
    fclose(file_pointer);
    return;
}

void io_write(uint16_t ADDRESS_BUS, uint8_t DATA_BUS) {
    if (~ADDRESS_BUS & 0x8000) {ram_contents[ADDRESS_BUS] = DATA_BUS;}
}
uint8_t io_read(uint16_t ADDRESS_BUS) {
    if (~ADDRESS_BUS & 0x8000) {return ram_contents[ADDRESS_BUS];}
    if (ADDRESS_BUS & 0x8000) {return rom_contents[ADDRESS_BUS - 0x8000];}
}