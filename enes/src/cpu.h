#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define STACK 0x0100
#define STACK_RESET 0xfd

typedef enum {
    CARRY             = 0b00000001,
    ZERO              = 0b00000010,
    INTERRUPT_DISABLE = 0b00000100,
    DECIMAL_MODE      = 0b00001000,
    BREAK             = 0b00010000,
    BREAK2            = 0b00100000,
    OVERFLOW          = 0b01000000,
    NEGATIV          = 0b10000000
} CpuFlag;

typedef struct {
    uint8_t register_a;
    uint8_t register_x;
    uint8_t register_y;
    CpuFlag status;
    uint16_t program_counter;
    uint8_t stack_pointer;
    uint8_t memory[0xffff];
} CPU;

void load(CPU* cpu, uint8_t* program, uint16_t size);
void run(CPU* cpu);
void reset(CPU* cpu);
void load_and_run(CPU* cpu, uint8_t* program, uint16_t size);

#endif // !CPU_H
