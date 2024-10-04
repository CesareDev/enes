#include <raylib.h>
#include <unistd.h>

#include "cpu.h"

void handle_input(CPU* cpu) {
    if (IsKeyDown(KEY_W)) {
        mem_write(cpu, 0xff, 0x77);
    }
    if (IsKeyDown(KEY_S)) {
        mem_write(cpu, 0xff, 0x73);
    }
    if (IsKeyDown(KEY_A)) {
        mem_write(cpu, 0xff, 0x61);
    }
    if (IsKeyDown(KEY_D)) {
        mem_write(cpu, 0xff, 0x64);
    }
}

Color color(uint8_t byte) {
    switch (byte) {
        case 0: return BLACK;
        case 1: return WHITE; 
        case 2:
        case 9: return LIGHTGRAY;
        case 3:
        case 10: return RED;
        case 4:
        case 11: return GREEN;
        case 5:
        case 12: return BLUE;
        case 6:
        case 13: return MAGENTA;
        case 7:
        case 14: return YELLOW;
        default: return PINK;
    }
}

bool read_screen_state(CPU* cpu, uint8_t* frame) {
    uint64_t frame_idx = 0;
    bool update = false;
    for (uint16_t i = 0x0200; i < 0x0600; i++) {
        uint8_t color_idx = mem_read(cpu, i);
        Color c = color(color_idx);
        uint8_t b1 = c.r;
        uint8_t b2 = c.g;
        uint8_t b3 = c.b;
        if (frame[frame_idx] != b1 || frame[frame_idx + 1] != b2 || frame[frame_idx + 2] != b3) {
            frame[frame_idx] = b1;
            frame[frame_idx + 1] = b2;
            frame[frame_idx + 2] = b3;
            update = true;
        }
        frame_idx += 3;
    }
    return update;
}

int main(int argc, char* argv[]) {

    InitWindow(320, 320, "Snake");
    SetRandomSeed(0);

    CPU cpu;
    Bus bus;
    RomResult result = load_rom("../../../enes/res/snake.nes");

    if (!result.valid) {
        CloseWindow();
        return 0;
    }

    init_cpu(&cpu, &bus, &result.rom);

    uint8_t screen[32 * 32 * 3] = { 0 };

    Image image = GenImageColor(32, 32, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);

    Texture2D texture_id;
    texture_id = LoadTextureFromImage(image);

    while (!WindowShouldClose() && cycle(&cpu)) { 
        handle_input(&cpu);
        uint8_t rand = GetRandomValue(1, 16);
        mem_write(&cpu, 0xfe, rand);
        if (read_screen_state(&cpu, screen)) {
            UpdateTexture(texture_id, screen);
        }
        BeginDrawing();
        DrawTexturePro(texture_id, (Rectangle){0.f, 0.f, 32.f, 32.f}, (Rectangle){ 0.f, 0.f, 320.f, 320.f }, (Vector2){ 0.f, 0.f }, 0.f, WHITE);
        EndDrawing();
        usleep(70);
    }

    UnloadTexture(texture_id);
    UnloadImage(image);
    CloseWindow();

    unload_rom(&result.rom);
    return 0;
}
