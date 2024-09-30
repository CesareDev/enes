#include "cpu.h"
#include "op_code.h"

#include <stdlib.h>

void update_zero_and_negative_flags(CPU* cpu, uint8_t result) {
    if (result == 0) {
        cpu->status = cpu->status | 0b00000010;
    } else {
        cpu->status = cpu->status & 0b11111101;
    }

    if ((result & 0b10000000) != 0) {
        cpu->status = cpu->status | 0b10000000;
    } else {
        cpu->status = cpu->status & 0b01111111;
    }
}

uint8_t mem_read(CPU* cpu, uint16_t address) {
    return cpu->memory[address];
}

void mem_write(CPU* cpu, uint16_t address, uint8_t data) {
    cpu->memory[address] = data;
}

uint16_t mem_read_uint16(CPU* cpu, uint16_t pos) {
    uint16_t lo = (uint16_t)mem_read(cpu, pos);
    uint16_t hi = (uint16_t)mem_read(cpu, pos + 1);
    return (hi << 8) | lo;
}

void mem_write_uint16(CPU* cpu, uint16_t pos, uint16_t data) {
    uint8_t hi = (uint8_t)(data >> 8);
    uint8_t lo = (uint8_t)(data & 0xFF);
    mem_write(cpu, pos, lo);
    mem_write(cpu, pos + 1, hi);
}

uint16_t get_operand_address(CPU* cpu, enum AddressingMode mode) {
    switch (mode) {
        case Immediate: return cpu->program_counter;
        case ZeroPage: return (uint16_t)mem_read(cpu, cpu->program_counter);
        case Absolute: return mem_read_uint16(cpu, cpu->program_counter);
        case ZeroPage_X: {
            uint8_t pos = mem_read(cpu, cpu->program_counter);
            uint8_t addr = (uint16_t)(pos + cpu->register_x);
            return addr;
        }
        case ZeroPage_Y: {
            uint8_t pos = mem_read(cpu, cpu->program_counter);
            uint8_t addr = (uint16_t)(pos + cpu->register_y);
            return addr;
        }
        case Absolute_X: {
            uint16_t base = mem_read_uint16(cpu, cpu->program_counter);
            uint16_t addr = base + (uint16_t)cpu->register_x;
            return addr;
        }
        case Absolute_Y: {
            uint16_t base = mem_read_uint16(cpu, cpu->program_counter);
            uint16_t addr = base + (uint16_t)cpu->register_y;
            return addr;
        }
        case Indirect_X: {
            uint8_t base = mem_read(cpu, cpu->program_counter);
            uint8_t ptr = (uint8_t)base + cpu->register_x;
            uint8_t lo = mem_read(cpu, (uint16_t)ptr);
            uint8_t hi = mem_read(cpu, (uint16_t)(ptr + 1));
            return (uint16_t)hi << 8 | (uint16_t)lo;
        }
        case Indirect_Y: {
            uint8_t base = mem_read(cpu, cpu->program_counter);
            uint8_t lo = mem_read(cpu, (uint16_t)base);
            uint8_t hi = mem_read(cpu, (uint16_t)((uint8_t)base + 1));
            uint16_t deref_base = (uint16_t)hi << 8 | (uint16_t)lo;
            uint16_t deref = deref_base + (uint16_t)cpu->register_y;
            return deref;
        }
        case NoneAddressing: {
            abort();
        }
    }
}

void lda(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_a = value;
    update_zero_and_negative_flags(cpu, cpu->register_a);
}

void tax(CPU* cpu) {
    cpu->register_x = cpu->register_a;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

void inx(CPU* cpu) {
    cpu->register_x++;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

void sta(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    mem_write(cpu, addr, cpu->register_a);
}

void load(CPU *cpu, uint8_t *program, uint16_t size) {
    for (uint16_t i = 0x8000; i < 0xFFFF && i < size; i++) {
        cpu->memory[i] = program[i - 0x8000];
    }
    cpu->program_counter = 0x8000;
}

void run(CPU *cpu) {
    while (1)
    {
        uint8_t op_code = mem_read(cpu, cpu->program_counter);
        cpu->program_counter += 1;

        switch (op_code) {
            // LDA
            case 0xA9: {
                lda(cpu, Immediate);
                cpu->program_counter += 1;
            }
            case 0xA5: {
                lda(cpu, ZeroPage);
                cpu->program_counter += 1;
            }
            case 0xAD: {
                lda(cpu, Absolute);
                cpu->program_counter += 2;
            }
            case 0xBD: {
                lda(cpu, Absolute_X);
                cpu->program_counter += 2;
            }
            case 0xB9: {
                lda(cpu, Absolute_Y);
                cpu->program_counter += 2;
            }
            case 0xA1: {
                lda(cpu, Indirect_X);
                cpu->program_counter += 1;
            }
            case 0xB1: {
                lda(cpu, Indirect_Y);
                cpu->program_counter += 1;
            }
            // STA
            case 0x85: {
                sta(cpu, Immediate);
                cpu->program_counter += 1;
            }
            case 0x95: {
                lda(cpu, ZeroPage);
                cpu->program_counter += 1;
            }
            case 0x8D: {
                lda(cpu, Absolute);
                cpu->program_counter += 2;
            }
            case 0x9D: {
                lda(cpu, Absolute_X);
                cpu->program_counter += 2;
            }
            case 0x99: {
                lda(cpu, Absolute_Y);
                cpu->program_counter += 2;
            }
            case 0x81: {
                lda(cpu, Indirect_X);
                cpu->program_counter += 1;
            }
            case 0x91: {
                lda(cpu, Indirect_Y);
                cpu->program_counter += 1;
            }
            case 0xAA: tax(cpu);
            case 0xe8: inx(cpu);
            case 0x00: return;
        }
    }
}

void reset(CPU* cpu) {
    cpu->register_a = 0;
    cpu->register_x = 0;
    cpu->register_y = 0;
    cpu->status = 0;
    cpu->program_counter = mem_read_uint16(cpu, 0xFFFC);
}

void load_and_run(CPU *cpu, uint8_t *program, uint16_t size) {
    load(cpu, program, size);
    reset(cpu);
    run(cpu);
}
