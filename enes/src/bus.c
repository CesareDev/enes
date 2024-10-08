#include "bus.h"
#include <stdlib.h>

void init_bus(Bus* bus, PPU* ppu, Rom* rom) {
    for (uint16_t i = 0; i < CPU_VRAM; i++) {
        bus->cpu_vram[i] = 0;
    }
    init_ppu(ppu, rom->chr_rom, rom->screen_mirroring);
    bus->ppu = ppu;
    bus->rom = rom;
    bus->cycles = 0;
}

bool bus_tick(Bus* bus, uint8_t cycles) {
    bus->cycles += (uint64_t)cycles;
    return ppu_tick(bus->ppu, cycles * 3);
}

NmiInterrupt poll_nmi_status(Bus* bus) {
    return poll_nmi_interrupt(bus->ppu);
}

uint8_t bus_mem_read(Bus* bus, uint16_t addr) {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0000011111111111;
        return bus->cpu_vram[mirror_down_address];
    } else if (addr == 0x2000 || addr == 0x2001 || addr == 0x2003 || addr == 0x2005 || addr == 0x2006 || addr == 0x4014) {
        return 0;
    } else if (addr == 0x2002) {
        return read_status(bus->ppu);
    } else if (addr == 0x2004) {
        return read_oam_data(bus->ppu);
    } else if (addr == 0x2007) {
        return read_data(bus->ppu);
    } else if (addr >= 0x4000 && addr <= 0x4015) {
        return 0;
    } else if (addr == 0x4016) {
        return 0;
    } else if (addr == 0x4017) {
        return 0;
    } else if (addr >= 0x2008 && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0010000000000111;
        return bus_mem_read(bus, mirror_down_address);
    } else if (addr >= 0x8000 && addr <= 0xffff) {
        return read_prg_rom(bus, addr);
    }
    return 0;
}

void bus_mem_write(Bus* bus, uint16_t addr, uint8_t data) {
    if (addr >= RAM && addr <= RAM_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b11111111111;
        bus->cpu_vram[mirror_down_address] = data;
    } else if (addr == 0x2000) {
        write_to_ctrl(bus->ppu, data);
    } else if (addr == 0x2001) {
        write_to_mask(bus->ppu, data);
    } else if (addr == 0x2002) {
        abort();
    } else if (addr == 0x2003) {
        write_to_oam_addr(bus->ppu, data);
    } else if (addr == 0x2004) {
        write_to_oam_data(bus->ppu, data);
    } else if (addr == 0x2005) {
        write_to_scroll(bus->ppu, data);
    } else if (addr == 0x2006) {
        write_to_ppu_addr(bus->ppu, data);
    } else if (addr == 0x2007) {
        write_to_data(bus->ppu, data);
    } else if (addr >= 0x2008 && addr <= PPU_REGISTERS_MIRRORS_END) {
        uint16_t mirror_down_address = addr & 0b0010000000000111;
        bus_mem_write(bus, addr, data);
    } else if ((addr >= 0x4000 && addr <= 0x4013) || addr == 0x4015) {
        //Ignore APU
    } else if (addr == 0x4014) {
        uint8_t buffer[256] = { 0 };
        uint16_t hi = (uint16_t)data << 8;
        for (uint16_t i = 0; i < 256; i++) {
            buffer[i] = bus_mem_read(bus, hi + i);
        }
        write_oam_dma(bus->ppu, buffer);
        // todo: handle this eventually
        // let add_cycles: u16 = if self.cycles % 2 == 1 { 514 } else { 513 };
        // self.tick(add_cycles); //todo this will cause weird effects as PPU will have 513/514 * 3 ticks
    } else if (addr == 0x4016) {
        //Ignore Joypad 1
    } else if (addr == 0x4017) {
        //Ignore Joypad 2
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
    if (bus->rom->prg_size == 0x4000 && addr >= 0x4000) {
        addr = addr % 0x4000;
    }
    return bus->rom->prg_rom[(uint64_t)addr];
}
