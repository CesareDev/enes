#include "registers.h"

void init_status_reg(StatusRegister* status_register) {
    *(status_register) = 0b00000000;
}

void set_vblank_status(StatusRegister* status_register, bool status) {
    if (status) {
        *(status_register) = *(status_register) | VBLANK_STARTED;
    } else {
        *(status_register) = *(status_register) & ~(VBLANK_STARTED);
    }
}

void set_sprite_zero_hit(StatusRegister* status_register, bool status) {
    if (status) {
        *(status_register) = *(status_register) | SPRITE_ZERO_HIT;
    } else {
        *(status_register) = *(status_register) & ~(SPRITE_ZERO_HIT);
    }
}

void set_sprite_overflow(StatusRegister* status_register, bool status) {
    if (status) {
        *(status_register) = *(status_register) | SPRITE_OVERFLOW;
    } else {
        *(status_register) = *(status_register) & ~(SPRITE_OVERFLOW);
    }
}

void reset_vblank_status(StatusRegister* status_register) {
    *(status_register) = *(status_register) & ~(VBLANK_STARTED);
}

bool is_in_vblank(StatusRegister* status_register) {
    return *(status_register) & VBLANK_STARTED;
}

uint8_t snapshot(StatusRegister status_register) {
    return status_register;
}
