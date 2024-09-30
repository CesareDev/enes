#ifndef CPU_H
#define CPU_H

#include <stdint.h>

typedef struct {
    uint8_t register_a;
    uint8_t register_x;
    uint8_t status;
    uint16_t program_counter;
} CPU;

void cpu_interpret(CPU* cpu, uint8_t* program);

#endif // !CPU_H
