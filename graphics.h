// Technically these are supposed to run asynchronously from one another.
// But since I'm lazy, I'm just going to have it display what's in VRAM, and figure out the hardware part when
// I actually get to it :)

#define TILE_DATA        0x4000
#define PALETTE_DATA     0x5000
#define LAYER_ONE        0x5080
#define LAYER_ONE_COL    0x6080
#define OBJECTS          0x7080

uint8_t tile_data[0x1000];                  // x256 2-bit colored tiles
uint8_t palette_data[0x80];                 // x32 2-bit colored information (4 bytes for 4 colors)  
Image frame_buffer;
Texture2D frame_texture;

void initialize_display() {
    frame_buffer = GenImageColor(512, 512, BLACK);
    frame_texture = LoadTextureFromImage(frame_buffer);
}

void update_tiledata() {return;}
void update_palette() {
    for (int x = PALETTE_DATA; x < PALETTE_DATA + 0x80; x++) {
        palette_data[x - PALETTE_DATA] = ram_contents[PALETTE_DATA + x];
    }
}

void display_vram() {
    // Displays layer one of the tiles.
    Color* pixels = (Color*)frame_buffer.data;

    // Loops through the tile references.
    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 64; x++) {
            
            // Goes to the tile references and reads the data from it.
            uint8_t tile_reference = ram_contents[LAYER_ONE + (y * 8) + x];
            for (int h = 0; h < 8; h++) {
                for (int s = 0; s < 8; s++) {
                    //printf("a\n");
                    uint8_t tile_a = ram_contents[TILE_DATA + (16*tile_reference) + h];
                    uint8_t tile_b = ram_contents[TILE_DATA + (16*tile_reference+8) + h];
                    //printf("b\n");
                    uint8_t mask = (0x01 << (7-s));
                    uint8_t tile_mask_a = (tile_a & mask) >> (7-s);
                    uint8_t tile_mask_b = (tile_b & mask) >> (7-(s+1));
                    //printf("c\n");
                    uint8_t color_index = tile_mask_a | tile_mask_b;
                    uint8_t color_reference = ram_contents[LAYER_ONE_COL + (y*8) + x];
                    uint8_t color = ram_contents[PALETTE_DATA + color_reference * 4 + color_index];
                    Color real_color = {
                        (int)(((color & 0b11100000) >> 5)/7.0 * 255), 
                        (int)(((color & 0b00011100) >> 2)/7.0 * 255), 
                        (int)(((color & 0b00000011))/3.0 * 255), 
                        0xff
                    };
                    //printf("d\n");
                    pixels[(y*8+h)*512 + (x*8+s)] = real_color;
                    //printf("e\n");
                }
            }
        }
    }
    UpdateTexture(frame_texture, frame_buffer.data);
    DrawTexture(frame_texture, 0, 0, WHITE);
}