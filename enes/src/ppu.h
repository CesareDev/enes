#ifndef PPU_H
#define PPU_H

#include <stdbool.h>
#include <stdint.h>

#include "mirroring.h"

#define PALETTE_SIZE 32
#define VRAM_SIZE 2048
#define OAM_SIZE 256

typedef struct {
    uint8_t value[2];
    bool hi_ptr;
} AddrRegister;

void init_addr_reg(AddrRegister* addr_reg);
void set_addr_reg(AddrRegister* addr_reg, uint16_t data);
uint16_t get_addr_reg(AddrRegister* addr_reg);
void update(AddrRegister* addr_reg, uint8_t data);
void increment(AddrRegister* addr_reg, uint8_t inc);
void reset_latch(AddrRegister* addr_reg);

typedef enum {
    NAMETABLE1              = 0b00000001,
    NAMETABLE2              = 0b00000010,
    VRAM_ADD_INCREMENT      = 0b00000100,
    SPRITE_PATTERN_ADDR     = 0b00001000,
    BACKROUND_PATTERN_ADDR  = 0b00010000,
    SPRITE_SIZE             = 0b00100000,
    MASTER_SLAVE_SELECT     = 0b01000000,
    GENERATE_NMI            = 0b10000000
} Register;

typedef uint8_t ControlRegister;

void init_control_register(ControlRegister* control_register);
uint8_t vram_addr_increment(ControlRegister control_register);

typedef struct {
    uint8_t* chr_rom;
    uint8_t palette_table[PALETTE_SIZE];
    uint8_t v_ram[VRAM_SIZE];
    uint8_t oam_data[OAM_SIZE];
    Mirroring mirroring;
    AddrRegister addr;
    ControlRegister ctrl;
    uint8_t internal_data_buf;
} PPU;

void init_ppu(PPU* ppu, uint8_t* chr_rom, Mirroring mirroring);
void write_to_ppu_addr(PPU* ppu, uint8_t value);
void write_to_ctrl(PPU* ppu, uint8_t value);
void increment_vram_addr(PPU* ppu);
uint8_t read_data(PPU* ppu);
uint16_t mirror_vram_addr(PPU* ppu, uint16_t addr);

#endif // !PPU_H
