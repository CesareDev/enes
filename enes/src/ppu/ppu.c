#include "ppu.h"
#include <stdlib.h>

void init_ppu(PPU* ppu, uint8_t* chr_rom, Mirroring mirroring) {
    ppu->chr_rom = chr_rom;
    ppu->mirroring = mirroring;
    for (uint8_t i = 0; i < PALETTE_SIZE; i++) {
        ppu->palette_table[i] = 0;
    }
    for (uint16_t i = 0; i < OAM_SIZE; i++) {
        ppu->oam_data[i] = 0;
    }
    for (uint16_t i = 0; i < VRAM_SIZE; i++) {
        ppu->v_ram[i] = 0;
    }
    init_control_reg(&ppu->ctrl);
    init_mask_reg(&ppu->mask);
    init_status_reg(&ppu->status);
    init_scroll_reg(&ppu->scroll);
    init_addr_reg(&ppu->addr);

    ppu->cycles = 0;
    ppu->scanline = 0;

    ppu->oam_addr = 0;
    ppu->internal_data_buf = 0;
    ppu->nmi_interrupt.value = 0;
    ppu->nmi_interrupt.is_valid = false;
}

void write_to_ctrl(PPU* ppu, uint8_t value) {
    bool before_nmi_status = generate_vblank_nmi(ppu->ctrl);
    update_control_reg(&ppu->ctrl, value);
    if (!before_nmi_status && generate_vblank_nmi(ppu->ctrl) && is_in_vblank(&ppu->status)) {
        ppu->nmi_interrupt.value = 1;
        ppu->nmi_interrupt.is_valid = true;
    }
}

void write_to_mask(PPU* ppu, uint8_t value) {
    update_mask_reg(&ppu->mask, value);
}

uint8_t read_status(PPU* ppu) {
    uint8_t data = snapshot(ppu->status);
    reset_vblank_status(&ppu->status);
    reset_latch_addr_reg(&ppu->addr);
    reset_latch_scroll_reg(&ppu->scroll);
    return data;
}

void write_to_oam_addr(PPU* ppu, uint8_t value) {
    ppu->oam_addr = value;
}

void write_to_oam_data(PPU* ppu, uint8_t value) {
    ppu->oam_data[ppu->oam_addr] = value;
    ppu->oam_addr++;
}

uint8_t read_oam_data(PPU* ppu) {
    return ppu->oam_data[ppu->oam_addr];
}

void write_to_scroll(PPU* ppu, uint8_t value) {
    write_scroll_reg(&ppu->scroll, value);
}

void write_to_ppu_addr(PPU* ppu, uint8_t value) {
    update_addr_reg(&ppu->addr, value);
}

void increment_vram_addr(PPU* ppu) {
    increment_addr_reg(&ppu->addr, vram_addr_increment(ppu->ctrl));
}

void write_to_data(PPU* ppu, uint8_t value) {
    uint16_t addr = get_addr_reg(&ppu->addr);
    if (addr >= 0 && addr <= 0x1fff) {
        abort();
    } else if (addr >= 0x2000 && addr <= 0x2fff) {
        ppu->v_ram[mirror_vram_addr(ppu, addr)] = value;
    } else if (addr >= 0x3000 && addr <= 0x3eff) {
        abort();
    } else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c) {
        uint16_t add_mirror = addr - 0x10;
        ppu->palette_table[add_mirror - 0x3f00] = value;
    } else if (addr >= 0x3f00 && addr <= 0x3fff) {
        ppu->palette_table[addr - 0x3f00] = value;
    } else {
        abort();
    }
    increment_vram_addr(ppu);
}

uint8_t read_data(PPU* ppu) {
    uint16_t addr = get_addr_reg(&ppu->addr);
    increment_vram_addr(ppu);
    if (addr >= 0 && addr <= 0x1fff) {
        uint8_t result = ppu->internal_data_buf;
        ppu->internal_data_buf = ppu->chr_rom[addr];
        return result;
    } else if (addr >= 0x2000 && addr <= 0x2fff) {
        uint8_t result = ppu->internal_data_buf;
        ppu->internal_data_buf = ppu->v_ram[mirror_vram_addr(ppu, addr)];
        return result;
    } else if (addr >= 0x3000 && addr <= 0x3eff) {
        abort();
    } else if (addr == 0x3f10 || addr == 0x3f14 || addr == 0x3f18 || addr == 0x3f1c) {
        uint16_t add_mirror = addr - 0x10;
        return ppu->palette_table[add_mirror - 0x3f00];
    } else if (addr >= 0x3f00 && addr <= 0x3fff) {
        return ppu->palette_table[(addr - 0x3fff)];
    }
    abort();
}

void write_oam_dma(PPU* ppu, uint8_t* value) {
    for (uint16_t i = 0; i < OAM_SIZE; i++) {
        ppu->oam_data[ppu->oam_addr] = value[i];
        ppu->oam_addr++;
    }
}

uint16_t mirror_vram_addr(PPU* ppu, uint16_t addr) {
    uint16_t mirrored_vram = addr & 0b10111111111111;
    uint16_t vram_index = mirrored_vram - 0x2000;
    uint16_t name_table = vram_index / 0x400;
    if ((ppu->mirroring == VERTICAL && name_table == 2) || (ppu->mirroring == VERTICAL && name_table == 3)) {
        return vram_index - 0x800;
    } else if (ppu->mirroring == HORIZONTAL) {
        if (name_table == 1) {
            return vram_index - 0x400;
        } else if (name_table == 2) {
            return vram_index - 0x400;
        } else if (name_table == 3) {
            return vram_index - 0x800;
        }
    }
    return vram_index;
}

bool ppu_tick(PPU* ppu, uint8_t cycles) {
    ppu->cycles += (uint64_t)cycles;
    if (ppu->cycles >= 341) {
        ppu->cycles = ppu->cycles - 341;
        ppu->scanline += 1;
        if (ppu->scanline == 241) {
            set_vblank_status(&ppu->status, true);
            set_sprite_zero_hit(&ppu->status, false);
            if (generate_vblank_nmi(ppu->ctrl)) {
                ppu->nmi_interrupt.value = 1;
                ppu->nmi_interrupt.is_valid = true;
            }
        }
        if (ppu->scanline >= 262) {
            ppu->scanline = 0;
            ppu->nmi_interrupt.value = 0;
            ppu->nmi_interrupt.is_valid = false;
            set_sprite_zero_hit(&ppu->status, false);
            reset_vblank_status(&ppu->status);
            return true;
        }
    }
    return false;
}

NmiInterrupt poll_nmi_interrupt(PPU* ppu) {
    NmiInterrupt interrupt = ppu->nmi_interrupt;
    ppu->nmi_interrupt.value = 0;
    ppu->nmi_interrupt.is_valid = false;
    return interrupt;
}
