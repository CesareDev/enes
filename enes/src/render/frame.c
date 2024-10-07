#include "frame.h"

void init_frame(Frame* frame) {
    for (uint32_t i = 0; i < FRAME_SIZE; i++) {
        frame->data[i] = 0;
    }
}

void set_pixel(Frame* frame, uint64_t x, uint64_t y, Vec3 rgb) {
    uint64_t base = y * 3 * WIDTH + x * 3;
    if ((base + 2) < FRAME_SIZE) {
        frame->data[base] = rgb.r;
        frame->data[base + 1] = rgb.g;
        frame->data[base + 2] = rgb.b;
    } 
}
