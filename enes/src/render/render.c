#include "render.h"

#include <stdlib.h>
#include "palette.h"

Palette bg_palette(PPU* ppu, uint8_t* attribute_table, uint64_t tile_column, uint64_t tile_row) {
    uint64_t attr_table_idx = (tile_row / 4) * 8 + tile_column / 4;
    uint8_t attr_byte = attribute_table[attr_table_idx];
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

typedef struct {
    uint64_t x1;
    uint64_t y1;
    uint64_t x2;
    uint64_t y2;
} Rect;

void render_name_table(PPU* ppu, Frame* frame, uint8_t* name_table, const Rect* view_port, int64_t shift_x, int64_t shift_y) {
    uint16_t bank = bknd_pattern_addr(ppu->ctrl);
    uint8_t* attribute_table = &name_table[0x3c0];
    for (uint16_t i = 0; i < 0x3c0; i++) {
        uint16_t tile_column = i % 32;
        uint16_t tile_row = i / 32;
        uint16_t tile_idx = (uint16_t)name_table[i];
        uint8_t* tile = &ppu->chr_rom[(bank + tile_idx * 16)];
        Palette palette = bg_palette(ppu, attribute_table, tile_column, tile_row);
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
                uint32_t pixel_x = tile_column * 8 + x;
                uint32_t pixel_y = tile_row * 8 + y;
                if (pixel_x >= view_port->x1 && 
                    pixel_x < view_port->x2 && 
                    pixel_y >= view_port->y1 && 
                    pixel_y < view_port->y2) {
                    set_pixel(
                        frame, 
                        (uint64_t)(shift_x + (int64_t)pixel_x),
                        (uint64_t)(shift_y + (int64_t)pixel_y),
                        rgb
                    );
                }
            }
        }
    }
}

void render(PPU* ppu, Frame* frame) {
    uint64_t scroll_x = (uint64_t)ppu->scroll.scroll_x;
    uint64_t scroll_y = (uint64_t)ppu->scroll.scroll_y;

    uint8_t* main_nametable = NULL;
    uint8_t* second_nametable = NULL;

    Mirroring mirr = ppu->mirroring;
    uint16_t n_add = nametable_addr(ppu->ctrl);

    if (mirr == VERTICAL) {
        if (n_add == 0x2000 || n_add == 0x2800) {
            main_nametable = ppu->v_ram;
            second_nametable = &ppu->v_ram[0x400];
        } else if (n_add == 0x2400 || n_add == 0x2c00) {
            main_nametable = &ppu->v_ram[0x400];
            second_nametable = ppu->v_ram;
        } 
    } else if (mirr == HORIZONTAL) {
        if (n_add == 0x2000 || n_add == 0x2400) {
            main_nametable = ppu->v_ram;
            second_nametable = &ppu->v_ram[0x400];
        } else if (n_add == 0x2800 || n_add == 0x2c00) {
            main_nametable = &ppu->v_ram[0x400];
            second_nametable = ppu->v_ram;
        }
    } else {
        abort();
    }

    Rect view_port = { scroll_x, scroll_y, WIDTH, HEIGHT };
    Rect view_port_x = { 0, 0, scroll_x, HEIGHT };
    Rect view_port_y = { 0, 0, WIDTH, scroll_y };

    render_name_table(ppu, frame, main_nametable, &view_port, -((int64_t)scroll_x), -((int64_t)scroll_y));

    if (scroll_x > 0) {
        render_name_table(ppu, frame, second_nametable, &view_port_x, (int64_t)(WIDTH - scroll_x), 0);
    } else if (scroll_y > 0) {
        render_name_table(ppu, frame, second_nametable, &view_port_y, 0, (int64_t)(HEIGHT - scroll_y));
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
                lower = lower >> 1;
                upper = upper >> 1;
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
