#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t value[2];
    bool hi_ptr;
} AddrRegister;

void init_addr_reg(AddrRegister* addr_reg);
void set_addr_reg(AddrRegister* addr_reg, uint16_t data);
uint16_t get_addr_reg(AddrRegister* addr_reg);
void update_addr_reg(AddrRegister* addr_reg, uint8_t data);
void increment_addr_reg(AddrRegister* addr_reg, uint8_t inc);
void reset_latch_addr_reg(AddrRegister* addr_reg);

typedef enum {
    NAMETABLE1              = 0b00000001,
    NAMETABLE2              = 0b00000010,
    VRAM_ADD_INCREMENT      = 0b00000100,
    SPRITE_PATTERN_ADDR     = 0b00001000,
    BACKROUND_PATTERN_ADDR  = 0b00010000,
    SPRITE_SIZE             = 0b00100000,
    MASTER_SLAVE_SELECT     = 0b01000000,
    GENERATE_NMI            = 0b10000000
} CtrlReg;

typedef uint8_t ControlRegister;

void init_control_reg(ControlRegister* control_register);
uint16_t nametable_addr(ControlRegister control_register);
uint8_t vram_addr_increment(ControlRegister control_register);
uint16_t sprt_pattern_addr(ControlRegister control_register);
uint16_t bknd_pattern_addr(ControlRegister control_register);
uint8_t sprite_size(ControlRegister control_register);
uint8_t master_slave_select(ControlRegister control_register);
bool generate_vblank_nmi(ControlRegister control_register);
void update_control_reg(ControlRegister* control_register, uint8_t data);

typedef enum {
    GRAYSCALE                = 0b00000001,
    LEFTMOST_8PXL_BACKGROUND = 0b00000010,
    LEFTMOST_8PXL_SPRITE     = 0b00000100,
    SHOW_BACKGROUND          = 0b00001000,
    SHOW_SPRITES             = 0b00010000,
    EMPHASISE_RED            = 0b00100000,
    EMPHASISE_GREEN          = 0b01000000,
    EMPHASISE_BLUE           = 0b10000000
} MskReg;

typedef enum {
    Null,
    Red,
    Green,
    Blue,
} RegColor;

typedef uint8_t EmphResult;

void insert_color(EmphResult* result, RegColor color);
bool contain_color(EmphResult result, RegColor color);

typedef uint8_t MaskRegister;

void init_mask_reg(MaskRegister* mask_register);
bool is_grayscale(MaskRegister mask_register);
bool leftmost_8pxl_background(MaskRegister mask_register);
bool leftmost_8pxl_sprite(MaskRegister mask_register);
bool show_background(MaskRegister mask_register);
bool show_sprites(MaskRegister mask_register);
EmphResult emphasise(MaskRegister mask_register);
void update_mask_reg(MaskRegister* mask_register, uint8_t data);

typedef struct {
    uint8_t scroll_x;
    uint8_t scroll_y;
    bool latch;
} ScrollRegister;

void init_scroll_reg(ScrollRegister* scroll_register);
void write_scroll_reg(ScrollRegister* scroll_register, uint8_t data);
void reset_latch_scroll_reg(ScrollRegister* scroll_register);

typedef enum {
    NOTUSED         = 0b00000001,
    NOTUSED2        = 0b00000010,
    NOTUSED3        = 0b00000100,
    NOTUSED4        = 0b00001000,
    NOTUSED5        = 0b00010000,
    SPRITE_OVERFLOW = 0b00100000,
    SPRITE_ZERO_HIT = 0b01000000,
    VBLANK_STARTED  = 0b10000000
} StatReg;

typedef uint8_t StatusRegister;

void init_status_reg(StatusRegister* status_register);
void set_vblank_status(StatusRegister* status_register, bool status); 
void set_sprite_zero_hit(StatusRegister* status_register, bool status);
void set_sprite_overflow(StatusRegister* status_register, bool status); 
void reset_vblank_status(StatusRegister* status_register);
bool is_in_vblank(StatusRegister* status_register);
uint8_t snapshot(StatusRegister status_register);

#endif // !REGISTERS_H
