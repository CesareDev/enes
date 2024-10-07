#include "registers.h"

void insert_color(EmphResult* result, RegColor color) {
    *(result) = *(result) | color;
}

bool contain_color(EmphResult result, RegColor color) {
    return result & color;
}

void init_mask_reg(MaskRegister* mask_register) {
    *(mask_register) = 0b00000000;
}

bool is_grayscale(MaskRegister mask_register) {
    return mask_register & GRAYSCALE;
}

bool leftmost_8pxl_background(MaskRegister mask_register) {
    return mask_register & LEFTMOST_8PXL_BACKGROUND;
}

bool leftmost_8pxl_sprite(MaskRegister mask_register) {
    return mask_register & LEFTMOST_8PXL_SPRITE;
}

bool show_background(MaskRegister mask_register) {
    return mask_register & SHOW_BACKGROUND;
}

bool show_sprites(MaskRegister mask_register) {
    return mask_register & SHOW_SPRITES;
}

EmphResult emphasise(MaskRegister mask_register) {
    EmphResult result = 0;
    if (mask_register & EMPHASISE_RED) {
        insert_color(&result, Red);
    }
    if (mask_register & EMPHASISE_BLUE) {
        insert_color(&result, Red);
    }
    if (mask_register & EMPHASISE_GREEN) {
        insert_color(&result, Green);
    }
    return result;
}

void update_mask_reg(MaskRegister* mask_register, uint8_t data) {
    *(mask_register) = data;
}
