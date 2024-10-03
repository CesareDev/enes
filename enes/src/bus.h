#ifndef BUS_H
#define BUS_H

#include "cartridge.h"
#include <stdint.h>

#define CPU_VRAM 2048
#define RAM 0x0000 
#define RAM_MIRRORS_END 0x1fff 
#define PPU_REGISTERS 0x2000 
#define PPU_REGISTERS_MIRRORS_END 0x3fff 

typedef struct {
    uint8_t cpu_vram[CPU_VRAM];
    Rom* rom;
} Bus;

void init_vram(Bus* bus);

uint8_t bus_mem_read(Bus* bus, uint16_t addr);
void bus_mem_write(Bus* bus, uint16_t addr, uint8_t data);
uint16_t bus_mem_read_u16(Bus* bus, uint16_t pos);
void bus_mem_write_u16(Bus* bus, uint16_t pos, uint16_t data);

uint8_t read_prg_rom(Bus* bus, uint16_t addr);

#endif // !BUS_H
