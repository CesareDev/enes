#include <raylib.h>
#include <stdlib.h>
#include <unistd.h>

#include "cpu.h"
#include "render/frame.h"
#include "render/palette.h"
#include "render/render.h"

void show_tile(Frame* frame, uint8_t* chr_rom, uint64_t bank, uint64_t tile_n) {
    if (bank > 1) abort();
    init_frame(frame);
    uint64_t v_bank = (uint64_t)(bank * 0x1000);
    uint8_t* tile = &chr_rom[(v_bank + tile_n * 16)];
    for (uint8_t y = 0; y < 8; y++) {
        uint8_t upper = tile[y];
        uint8_t lower = tile[y + 8];
        for (int8_t x = 7; x >= 0; x--) {
            uint8_t value = (1 & upper) << 1 | (1 & lower);
            upper = upper >> 1;
            lower = lower >> 1;
            Vec3 rgb;
            switch (value) {
                case 0: rgb = SYSTEM_PALETTE[0x01]; break;
                case 1: rgb = SYSTEM_PALETTE[0x23]; break;
                case 2: rgb = SYSTEM_PALETTE[0x27]; break;
                case 3: rgb = SYSTEM_PALETTE[0x30]; break;
                default: abort();
            }
            set_pixel(frame, x, y, rgb);
        }
    }
}

void show_tile_bank(Frame* frame, uint8_t* chr_rom, uint64_t bank) {
    if (bank > 1) abort();
    init_frame(frame);
    uint32_t tile_y = 0;
    uint32_t tile_x = 0;
    uint64_t v_bank = (uint64_t)(bank * 0x1000);
    for (uint8_t tile_n = 0; tile_n < 255; tile_n++) {
        if (tile_n != 0 && tile_n % 20 == 0) {
            tile_y += 10;
            tile_x = 0;
        }
        uint8_t* tile = &chr_rom[(v_bank + tile_n * 16)];
        for (uint8_t y = 0; y < 8; y++) {
            uint8_t upper = tile[y];
            uint8_t lower = tile[y + 8];
            for (int8_t x = 7; x >= 0; x--) {
                uint8_t value = (1 & upper) << 1 | (1 & lower);
                upper = upper >> 1;
                lower = lower >> 1;
                Vec3 rgb;
                switch (value) {
                    case 0: rgb = SYSTEM_PALETTE[0x01]; break;
                    case 1: rgb = SYSTEM_PALETTE[0x23]; break;
                    case 2: rgb = SYSTEM_PALETTE[0x27]; break;
                    case 3: rgb = SYSTEM_PALETTE[0x30]; break;
                    default: abort();
                }
                set_pixel(frame, tile_x + x, tile_y + y, rgb);
            }
        }
        tile_x += 10;
    }
}

PPU* ppu_p;
Frame* frame_p;
Texture2D* texture_id_p;

void bus_callback() {
    render(ppu_p, frame_p);
    UpdateTexture(*texture_id_p, frame_p->data);
}

int main(int argc, char* argv[]) {

    InitWindow(256 * 3, 240 * 3, "Pacman");
    //SetTargetFPS(60);

    RomResult result = load_rom("../../../enes/res/pacman.nes");
    if (!result.valid) {
        CloseWindow();
        return 0;
    }

    CPU cpu;
    PPU ppu;
    Bus bus;
    Frame frame;
    Texture2D texture_id;

    frame_p = &frame;
    ppu_p = &ppu;
    texture_id_p = &texture_id;

    init(&cpu, &ppu, &bus, &result.rom, bus_callback);
    init_frame(&frame);

    Image image = GenImageColor(256, 240, BLACK);
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8);
    texture_id = LoadTextureFromImage(image);

    while (!WindowShouldClose()) {
        if (!cycle(&cpu)) break;
        BeginDrawing();
        DrawTexturePro(
            texture_id, 
            (Rectangle){ 0.f, 0.f, 256.f, 240.f }, 
            (Rectangle){ 0.f, 0.f, 256.f * 3.f, 240.f * 3.f }, 
            (Vector2){ 0.f, 0.f }, 
            0.f, 
            WHITE
        );
        EndDrawing();
    }

    UnloadTexture(texture_id);
    UnloadImage(image);
    CloseWindow();

    unload_rom(&result.rom);
    return 0;
}
