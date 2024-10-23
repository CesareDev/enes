#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>

#include "bus.h"

#define STACK 0x0100
#define STACK_RESET 0xfd

typedef enum {
    NMI
} InterruptType;

typedef struct {
    InterruptType itype;
    uint16_t vector_addr;
    uint8_t b_flag_mask;
    uint8_t cpu_cycles;
} Interrupt;

typedef enum {
    CARRY             = 0b00000001,
    ZERO              = 0b00000010,
    INTERRUPT_DISABLE = 0b00000100,
    DECIMAL_MODE      = 0b00001000,
    BREAK             = 0b00010000,
    BREAK2            = 0b00100000,
    OVERFLOW          = 0b01000000,
    NEGATIV           = 0b10000000
} Flag;

typedef uint8_t CpuFlag;

typedef struct {
    uint8_t register_a;
    uint8_t register_x;
    uint8_t register_y;
    CpuFlag status;
    uint16_t program_counter;
    uint8_t stack_pointer;
    Bus* bus;
} CPU;

void mem_write(CPU* cpu, uint16_t addr, uint8_t data);
uint8_t mem_read(CPU* cpu, uint16_t addr);

void init_cpu(CPU* cpu, Bus* bus);

void load(CPU* cpu, uint8_t* program, uint16_t size);
bool cycle(CPU* cpu);
void reset(CPU* cpu);
void load_and_reset(CPU* cpu, uint8_t* program, uint16_t size);
void run(CPU* cpu, bool condition);

#endif // !CPU_H
