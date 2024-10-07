#include "registers.h"
#include <stdlib.h>

void init_control_reg(ControlRegister* control_register) {
    *(control_register) = 0b00000000;
}

uint16_t nametable_addr(ControlRegister control_register) {
    switch (control_register & 0b11) {
        case 0: return 0x2000; break;
        case 1: return 0x2400; break;
        case 2: return 0x2800; break;
        case 3: return 0x2c00; break;
        default: abort();
    }
}

uint8_t vram_addr_increment(ControlRegister control_register) {
    if (!(control_register & VRAM_ADD_INCREMENT)) {
        return 1;
    }
    return 32;
}

uint16_t sprt_pattern_addr(ControlRegister control_register) {
    if (!(control_register & SPRITE_PATTERN_ADDR)) {
        return 0;
    }
    return 0x1000;
}

uint16_t bknd_pattern_addr(ControlRegister control_register) {
    if (!(control_register & BACKROUND_PATTERN_ADDR)) {
        return 0;
    }
    return 0x1000;
}

uint8_t sprite_size(ControlRegister control_register) {
    if (!(control_register & SPRITE_SIZE)) {
        return 8;
    }
    return 16;
}

uint8_t master_slave_select(ControlRegister control_register) {
    if (!(control_register & MASTER_SLAVE_SELECT)) {
        return 0;
    }
    return 1;
}

bool generate_vblank_nmi(ControlRegister control_register) {
    return control_register & GENERATE_NMI;
}

void update_control_reg(ControlRegister* control_register, uint8_t data) {
    *(control_register) = data;
}
