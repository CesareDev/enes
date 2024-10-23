#include <raylib.h>
#include <unistd.h>

#include "cpu.h"
#include "render/frame.h"
#include "render/render.h"

PPU* ppu_p;
Joypad* joypad_p;
Frame* frame_p;
Texture2D* texture_id_p;

void bus_callback() {
    render(ppu_p, frame_p);
    UpdateTexture(*texture_id_p, frame_p->data);
}

int main(int argc, char* argv[]) {

    InitWindow(256 * 3, 240 * 3, "enes");
    //SetTargetFPS(60);

    RomResult rom_result = load_rom("../../../enes/res/mario.nes");
    if (!rom_result.valid) {
        CloseWindow();
        return 0;
    }

    CPU cpu;
    PPU ppu;
    Bus bus;
    Joypad joypad;

    Frame frame;
    Texture2D texture_id;

    frame_p = &frame;
    ppu_p = &ppu;
    texture_id_p = &texture_id;
    joypad_p = &joypad;

    init_bus(&bus, &ppu, &rom_result.rom, &joypad, bus_callback);
    init_ppu(&ppu, rom_result.rom.chr_rom, rom_result.rom.screen_mirroring);
    init_cpu(&cpu, &bus);
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

    unload_rom(&rom_result.rom);
    return 0;
}
