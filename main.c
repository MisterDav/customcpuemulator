#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include "include/raylib.h"
#include "cpu.h"
#include "graphics.h"

int main() {

    struct timeb time_last, time_now;
    ftime(&time_last);

    InitWindow(512, 512, "Dobe.8 - v0.1");
    SetTargetFPS(0);
    load_rom("main.bin");
    initialize_display();

    while (!WindowShouldClose()) {
        
        //printf("%u\n", time_last.millitm);

        ftime(&time_now);
        int time_to_ms = (int)(time_now.time - time_last.time) * 1000 + (int)(time_now.millitm - time_last.millitm);
        if (time_to_ms <= 16) {
            BeginDrawing();
            EndDrawing();

        } else {
            BeginDrawing();
            ClearBackground(WHITE);
            display_vram();
            EndDrawing();
            ftime(&time_last);
        }

        // Check for keypresses
        int keypress = GetKeyPressed();
        if (keypress != 0) {
            ram_contents[0x3FF9] = keypress;
        }

        decode_microinstruction(CLOCK);
        cpu_update();
        CLOCK = !CLOCK;
        //if (CLOCK) {debug(3);}


    }
    ram_dump();
    CloseWindow();
    return 0;
}