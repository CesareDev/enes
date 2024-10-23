#ifndef JOYPAD_H
#define JOYPAD_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    RIGHT             = 0b10000000,
    LEFT              = 0b01000000,
    DOWN              = 0b00100000,
    UP                = 0b00010000,
    START             = 0b00001000,
    SELECT            = 0b00000100,
    BUTTON_B          = 0b00000010,
    BUTTON_A          = 0b00000001
} JoypadButtonEn;

typedef uint8_t JoypadButton;

typedef struct {
    bool strobe;
    uint8_t button_index;
    JoypadButton button_status;
} Joypad;

void init_joypad(Joypad* joypad);
void joypad_write(Joypad* joypad, uint8_t data);
uint8_t joypad_read(Joypad* joypad);
void set_button_pressed_status(Joypad* joypad, JoypadButton button, bool pressed);

#endif // !JOYPAD_H
