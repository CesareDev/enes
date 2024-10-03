#include "bus.h"
#include <stdlib.h>

void init_vram(Bus* bus) {
    for (uint16_t i = 0; i < CPU_VRAM; i++) {
        bus->cpu_vram[i] = 0;
    }
}

uint8_t bus_mem_read(Bus* bus, uint16_t addr) {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0000011111111111;
        return bus->cpu_vram[mirror_down_address];
    } else if (addr >= PPU_REGISTERS && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0010000000000111;
        abort();
    } else if (addr >= 0x8000 && addr <= 0xffff) {
        return read_prg_rom(bus, addr);
    }
    return 0;
}

void bus_mem_write(Bus* bus, uint16_t addr, uint8_t data) {
    switch (addr) {
        case RAM ... RAM_MIRRORS_END: {
            uint16_t mirror_down_address = addr & 0b0000011111111111;
            bus->cpu_vram[mirror_down_address] = data;
            break;
        }
        case PPU_REGISTERS ... PPU_REGISTERS_MIRRORS_END: {
            uint16_t mirror_down_address = addr & 0b0010000000000111;
            abort();
        }
        case 0x8000 ... 0xffff: abort();
        default: break;
    }
}

uint16_t bus_mem_read_u16(Bus* bus, uint16_t pos) {
    uint16_t lo = (uint16_t)bus_mem_read(bus, pos);
    uint16_t hi = (uint16_t)bus_mem_read(bus, pos + 1);
    return (hi << 8) | lo;
}

void bus_mem_write_u16(Bus* bus, uint16_t pos, uint16_t data) {
    uint8_t hi = (uint8_t)(data >> 8);
    uint8_t lo = (uint8_t)(data & 0xff);
    bus_mem_write(bus, pos, lo);
    bus_mem_write(bus, pos + 1, hi);
}

uint8_t read_prg_rom(Bus* bus, uint16_t addr) {
    addr -= 0x8000;
    if (sizeof(bus->rom->prg_rom) == 0x4000 && addr >= 0x4000) {
        addr = addr % 0x4000;
    }
    return bus->rom->prg_rom[(uint64_t)addr];
}
