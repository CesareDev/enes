#include "render.h"

#include <stdlib.h>
#include "palette.h"

Palette bg_palette(PPU* ppu, uint64_t tile_column, uint64_t tile_row) {
    uint64_t attr_table_idx = (tile_row / 4) * 8 + tile_column / 4;
    uint8_t attr_byte = ppu->v_ram[0x3c0 + attr_table_idx];
    uint8_t palette_idx = 0;
    uint64_t col = (tile_column % 4) / 2;
    uint64_t row = (tile_row % 4) / 2;
    if (col == 0 && row == 0) {
        palette_idx = attr_byte & 0b11;
    } else if (col == 1 && row == 0) {
        palette_idx = (attr_byte >> 2) & 0b11;
    } else if (col == 0 && row == 1) {
        palette_idx = (attr_byte >> 4) & 0b11;
    } else if (col == 1 && row == 1) {
        palette_idx = (attr_byte >> 6) & 0b11;
    } else {
        abort();
    }
    uint64_t palette_start = 1 + (uint64_t)palette_idx * 4;
    return (Palette){ 
        ppu->palette_table[0], 
        ppu->palette_table[palette_start], 
        ppu->palette_table[palette_start + 1],
        ppu->palette_table[palette_start + 2]
    };
}

Palette sprite_palette(PPU* ppu, uint8_t palette_idx) {
    uint64_t start = 0x11 + (uint64_t)(palette_idx * 4);
    return (Palette){
        0,
        ppu->palette_table[start], 
        ppu->palette_table[start + 1],
        ppu->palette_table[start + 2]
    };
}

void render(PPU* ppu, Frame* frame) {
    uint16_t bank = bknd_pattern_addr(ppu->ctrl);
    for (uint16_t i = 0; i < 0x3c0; i++) {
        uint16_t v_tile = (uint16_t)ppu->v_ram[i];
        uint16_t tile_column = i % 32;
        uint16_t tile_row = i / 32;
        uint8_t* tile = &ppu->chr_rom[(bank + v_tile * 16)];
        Palette palette = bg_palette(ppu, tile_column, tile_row);
        for (uint8_t y = 0; y < 8; y++) {
            uint8_t upper = tile[y];
            uint8_t lower = tile[y + 8];
            for (int8_t x = 7; x >= 0; x--) {
                uint8_t value = (1 & lower) << 1 | (1 & upper);
                upper = upper >> 1;
                lower = lower >> 1;
                Vec3 rgb;
                switch (value) {
                    case 0: rgb = SYSTEM_PALETTE[ppu->palette_table[0]]; break;
                    case 1: rgb = SYSTEM_PALETTE[palette.palette_1]; break;
                    case 2: rgb = SYSTEM_PALETTE[palette.palette_2]; break;
                    case 3: rgb = SYSTEM_PALETTE[palette.palette_3]; break;
                    default: abort();
                }
                set_pixel(frame, tile_column * 8 + x, tile_row * 8 + y, rgb);
            }
        }
    }
    for (int16_t i = OAM_SIZE - 1; i >= 0; i -= 4) {
        uint16_t tile_idx = (uint16_t)ppu->oam_data[i + 1];
        uint64_t tile_x = (uint64_t)ppu->oam_data[i + 3];
        uint64_t tile_y = (uint64_t)ppu->oam_data[i];
        bool flip_vertical = (((ppu->oam_data[i + 2] >> 7) & 1) == 1) ? true : false;
        bool flip_horizontal = (((ppu->oam_data[i + 2] >> 6) & 1) == 1) ? true : false;
        uint8_t palette_idx = ppu->oam_data[i + 2] & 0b11;
        Palette s_palette = sprite_palette(ppu, palette_idx);
        uint16_t bank = sprt_pattern_addr(ppu->ctrl);
        uint8_t* tile = &ppu->chr_rom[(bank + tile_idx * 16)];
        for (uint8_t y = 0; y < 8; y++) {
            uint8_t upper = tile[y];
            uint8_t lower = tile[y + 8];
            for (int8_t x = 7; x >= 0; x--) {
                uint8_t value = (1 & lower) << 1 | (1 & upper);
                upper = upper >> 1;
                lower = lower >> 1;
                Vec3 rgb;
                switch (value) {
                    case 0: continue;
                    case 1: rgb = SYSTEM_PALETTE[s_palette.palette_1]; break;
                    case 2: rgb = SYSTEM_PALETTE[s_palette.palette_2]; break;
                    case 3: rgb = SYSTEM_PALETTE[s_palette.palette_3]; break;
                    default: abort();
                }
                if (!flip_horizontal && !flip_vertical) {
                    set_pixel(frame, tile_x + x, tile_y + y, rgb);
                } else if (flip_horizontal && !flip_vertical) {
                    set_pixel(frame, tile_x + 7 - x, tile_y + y, rgb);
                } else if (!flip_horizontal && flip_vertical) {
                    set_pixel(frame, tile_x + x, tile_y + 7 - y, rgb);
                } else if (flip_horizontal && flip_vertical) {
                    set_pixel(frame, tile_x + 7 - x, tile_y + 7 - y, rgb);
                }
            }
        }
    }
}
