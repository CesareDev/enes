#ifndef FRAME_H
#define FRAME_H

#include "vec.h"

#define WIDTH 256
#define HEIGHT 240
#define FRAME_SIZE 256 * 240 * 3

typedef struct {
    uint8_t data[FRAME_SIZE];
} Frame;

void init_frame(Frame* frame);
void set_pixel(Frame* frame, uint64_t x, uint64_t y, Vec3 rgb);

#endif // !FRAME_H
