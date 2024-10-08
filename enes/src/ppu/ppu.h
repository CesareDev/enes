#ifndef PPU_H
#define PPU_H

#include <stdbool.h>
#include <stdint.h>

#include "../mirroring.h"
#include "registers.h"

#define PALETTE_SIZE 32
#define VRAM_SIZE 2048
#define OAM_SIZE 256

typedef struct {
    uint8_t value;
    bool is_valid;
} NmiInterrupt;

typedef struct {
    uint8_t* chr_rom;
    Mirroring mirroring;
    ControlRegister ctrl;
    MaskRegister mask;
    StatusRegister status;
    ScrollRegister scroll;
    AddrRegister addr;
    uint8_t v_ram[VRAM_SIZE];
    uint8_t oam_addr;
    uint8_t oam_data[OAM_SIZE];
    uint8_t palette_table[PALETTE_SIZE];
    uint8_t internal_data_buf;
    uint16_t scanline;
    uint64_t cycles;
    NmiInterrupt nmi_interrupt;
} PPU;

void init_ppu(PPU* ppu, uint8_t* chr_rom, Mirroring mirroring);
void write_to_ctrl(PPU* ppu, uint8_t value);
void write_to_mask(PPU* ppu, uint8_t value);
uint8_t read_status(PPU* ppu);
void write_to_oam_addr(PPU* ppu, uint8_t value);
void write_to_oam_data(PPU* ppu, uint8_t value);
uint8_t read_oam_data(PPU* ppu);
void write_to_scroll(PPU* ppu, uint8_t value);
void write_to_ppu_addr(PPU* ppu, uint8_t value);
void write_to_data(PPU* ppu, uint8_t value);
uint8_t read_data(PPU* ppu);
void write_oam_dma(PPU* ppu, uint8_t* value);

uint16_t mirror_vram_addr(PPU* ppu, uint16_t addr);
bool ppu_tick(PPU* ppu, uint8_t cycles);
NmiInterrupt poll_nmi_interrupt(PPU* ppu);

#endif // !PPU_H
