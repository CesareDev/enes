#include "registers.h"

void init_scroll_reg(ScrollRegister* scroll_register) {
    scroll_register->scroll_x = 0;
    scroll_register->scroll_y = 0;
    scroll_register->latch = false;
}

void write_scroll_reg(ScrollRegister* scroll_register, uint8_t data) {
    if (!scroll_register->latch) {
        scroll_register->scroll_x = data;
    } else {
        scroll_register->scroll_y = data;
    }
    scroll_register->latch = !scroll_register->latch;
}

void reset_latch_scroll_reg(ScrollRegister* scroll_register) {
    scroll_register->latch = false;
}
