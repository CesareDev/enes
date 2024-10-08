#ifndef RENDER_H
#define RENDER_H

#include <stdint.h>
#include "../ppu/ppu.h"
#include "frame.h"

typedef struct {
    uint8_t palette_0;
    uint8_t palette_1;
    uint8_t palette_2;
    uint8_t palette_3;
} Palette;

void render(PPU* ppu, Frame* frame);

#endif // !RENDER_H
