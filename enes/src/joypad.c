#include "joypad.h"

void init_joypad(Joypad* joypad) {
    joypad->strobe = false;
    joypad->button_index = 0;
    joypad->button_status = 0b0;
}

void joypad_write(Joypad* joypad, uint8_t data) {
    joypad->strobe = (data & 1) == 1;
    if (joypad->strobe) {
        joypad->button_index = 0;
    }
}

uint8_t joypad_read(Joypad* joypad) {
    if (joypad->button_index > 7) {
        return 1;
    }
    uint8_t response = (joypad->button_status & (1 << joypad->button_index)) >> joypad->button_index;
    if (!joypad->strobe && joypad->button_index <= 7) {
        joypad->button_index += 1;
    }
    return response;
}

void set_button(Joypad* joypad, JoypadButton button, bool condition) {
    if (condition) {
        joypad->button_status = joypad->button_status | button;
    } else {
       joypad->button_status = joypad->button_status & ~button;
    }
}

void set_button_pressed_status(Joypad* joypad, JoypadButton button, bool pressed) {
    set_button(joypad, button, pressed);
}
