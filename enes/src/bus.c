#include "bus.h"
#include <stdlib.h>

void init_bus(Bus* bus, PPU* ppu, Rom* rom) {
    for (uint16_t i = 0; i < CPU_VRAM; i++) {
        bus->cpu_vram[i] = 0;
    }
    init_ppu(ppu, rom->chr_rom, rom->screen_mirroring);
    bus->ppu = ppu;
    bus->rom = rom;
}

uint8_t bus_mem_read(Bus* bus, uint16_t addr) {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0000011111111111;
        return bus->cpu_vram[mirror_down_address];
    } else if (addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || addr == 0x2005 || addr == 0x2006 || addr == 0x4014) {
        abort();
    } else if (addr == 0x2007) {
        return read_data(bus->ppu);
    } else if (addr >= 0x2008 && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0010000000000111;
        bus_mem_read(bus, mirror_down_address);
    } else if (addr >= 0x8000 && addr <= 0xffff) {
        return read_prg_rom(bus, addr);
    }
    return 0;
}

void bus_mem_write(Bus* bus, uint16_t addr, uint8_t data) {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0000011111111111;
        bus->cpu_vram[mirror_down_address] = data;
    } else if (addr == 0x2000) {
        write_to_ctrl(bus->ppu, data);
    } else if (addr == 0x2006) {
        write_to_ppu_addr(bus->ppu, data);
    } else if (addr == 0x2007) {
        //TODO
    } else if (addr >= 0x2008 && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0010000000000111;
        bus_mem_read(bus, mirror_down_address);
    } else if (addr >= 0x8000 && addr <= 0xffff) {
        abort();
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
