#include "ppu.h"
#include <stdlib.h>

void init_addr_reg(AddrRegister* addr_reg) {
    addr_reg->value[0] = 0;
    addr_reg->value[1] = 0;
    addr_reg->hi_ptr = true;
}

void set_addr_reg(AddrRegister* addr_reg, uint16_t data) {
    addr_reg->value[0] = (uint8_t)(data >> 8);
    addr_reg->value[1] = (uint8_t)(data & 0xff);
}

uint16_t get_addr_reg(AddrRegister* addr_reg) {
    return (uint16_t)(addr_reg->value[0] << 8) | (uint16_t)addr_reg->value[1];
}

void update(AddrRegister* addr_reg, uint8_t data) {
    if (addr_reg->hi_ptr) {
        addr_reg->value[0] = data;
    } else {
        addr_reg->value[1] = data;
    }

    if (get_addr_reg(addr_reg) > 0x3fff) {
        set_addr_reg(addr_reg, get_addr_reg(addr_reg) & 0b11111111111111);
    }

    addr_reg->hi_ptr = !addr_reg->hi_ptr;
}

void increment(AddrRegister* addr_reg, uint8_t inc) {
    uint8_t lo = addr_reg->value[1];
    addr_reg->value[1] = addr_reg->value[1] + inc;
    if (lo > addr_reg->value[1]) {
        addr_reg->value[0] = addr_reg->value[0] + 1;
    }
    if (get_addr_reg(addr_reg) > 0x3fff) {
        set_addr_reg(addr_reg, get_addr_reg(addr_reg) & 0b11111111111111);
    }
}

void reset_latch(AddrRegister* addr_reg) {
    addr_reg->hi_ptr = true;
}

void init_control_register(ControlRegister* control_register) {
    *control_register = 0b00000000;
}

uint8_t vram_addr_increment(ControlRegister control_register) {
    if (!(control_register & VRAM_ADD_INCREMENT)) {
        return 1;
    }
    return 32;
}

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
}

void write_to_ppu_addr(PPU* ppu, uint8_t value) {
    update(&ppu->addr, value);
}

void write_to_ctrl(PPU* ppu, uint8_t value) {
    ppu->ctrl = value;
}

void increment_vram_addr(PPU* ppu) {
    increment(&ppu->addr, vram_addr_increment(ppu->ctrl));
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
    } else if (addr >= 0x3f00 && addr <= 0x3fff) {
        return ppu->palette_table[(addr - 0x3fff)];
    }
    abort();
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
