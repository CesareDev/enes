#include "cpu.h"

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

void lda(CPU* cpu, uint8_t value) {
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

void cpu_interpret(CPU *cpu, uint8_t *program) {
    cpu->program_counter = 0;

    while (1)
    {
        uint8_t op_code = program[cpu->program_counter];
        cpu->program_counter++;

        switch (op_code) {
            case 0xA9: {
                uint8_t param = program[cpu->program_counter];
                cpu->program_counter++;

                lda(cpu, param);
            }
            case 0xAA: tax(cpu);
            case 0xe8: inx(cpu);
            case 0x00: return;
        }
    }
}
