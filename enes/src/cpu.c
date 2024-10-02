#include "cpu.h"
#include "op_code.h"

#include <stdlib.h>
#include <stdbool.h>

void insert(CpuFlag* status, CpuFlag flag) {
    *status = *status | flag;
}

void remove(CpuFlag* status, CpuFlag flag) {
    *status = *status & (0xff ^ flag);
}

bool contains(CpuFlag status, CpuFlag flag) {
    return (status & flag) > 0; 
}

void update_zero_and_negative_flags(CPU* cpu, uint8_t result) {
    if (result == 0) {
        insert(&cpu->status, ZERO);
    } else {
        remove(&cpu->status, ZERO);
    }

    if ((result & 0b10000000) != 0) {
        insert(&cpu->status, NEGATIV);
    } else {
        remove(&cpu->status, NEGATIV);
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
    uint8_t lo = (uint8_t)(data & 0xff);
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

void ldy(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_y = value;
    update_zero_and_negative_flags(cpu, cpu->register_y);
}

void ldx(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_x = value;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

void lda(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_a = value;
    update_zero_and_negative_flags(cpu, cpu->register_a);
}

void sta(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    mem_write(cpu, addr, cpu->register_a);
}

void set_register_a(CPU* cpu, uint8_t value) {
    cpu->register_a = value;
    update_zero_and_negative_flags(cpu, cpu->register_a);
}

void and(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    set_register_a(cpu, data & cpu->register_a);
}

void eor(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    set_register_a(cpu, data ^ cpu->register_a);
}

void ora(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    set_register_a(cpu, data | cpu->register_a);
}

void tax(CPU* cpu) {
    cpu->register_x = cpu->register_a;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

void inx(CPU* cpu) {
    cpu->register_x++;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

void iny(CPU* cpu) {
    cpu->register_y++;
    update_zero_and_negative_flags(cpu, cpu->register_y);
}

void load_and_run(CPU *cpu, uint8_t *program, uint16_t size) {
    populate_op_index();
    load(cpu, program, size);
    reset(cpu);
    run(cpu);
}

void load(CPU *cpu, uint8_t *program, uint16_t size) {
    for (uint16_t i = 0x8000; i < 0xffff && (i - 0x8000) < size; i++) {
        cpu->memory[i] = program[i - 0x8000];
    }
    mem_write_uint16(cpu, 0xfffc, 0x8000);
}

void reset(CPU* cpu) {
    cpu->register_a = 0;
    cpu->register_x = 0;
    cpu->register_y = 0;
    cpu->status = 0;
    cpu->stack_pointer = STACK_RESET;
    cpu->status = 0b100100;
    cpu->program_counter = mem_read_uint16(cpu, 0xfffc);
}

void set_carry_flag(CPU* cpu) {
    insert(&cpu->status, CARRY);
}

void clear_carry_flag(CPU* cpu) {
    remove(&cpu->status, CARRY);
}

void add_to_register_a(CPU* cpu, uint8_t data) {
    uint16_t has_carry = contains(cpu->status, CARRY);
    uint16_t sum = (uint16_t)cpu->register_a + (uint16_t)data + has_carry;
    bool carry = sum > 0xff;
    if (carry) {
        insert(&cpu->status, CARRY);
    } else {
        remove(&cpu->status, CARRY);
    }
    uint8_t result = (uint8_t)sum;
    if (((data ^ result) & (result & cpu->register_a) & 0x80) != 0) {
        insert(&cpu->status, OVERFLOW);
    } else {
        remove(&cpu->status, OVERFLOW);
    }
    set_register_a(cpu, result);
}

void sbc(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    // ChatGPT tell this NOT SURE!
    int8_t tmp_resul = -(data) - 1;
    uint8_t result = (uint8_t)tmp_resul;
    add_to_register_a(cpu, result);
}

void adc(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    add_to_register_a(cpu, value);
}

uint8_t stack_pop(CPU* cpu) {
    cpu->stack_pointer++;
    return mem_read(cpu, (uint16_t)STACK + (uint16_t)cpu->stack_pointer);
}

void stack_push(CPU* cpu, uint8_t data) {
    mem_write(cpu, (uint16_t)STACK + (uint16_t)cpu->stack_pointer, data);
    cpu->stack_pointer--;
}

void stack_push_u16(CPU* cpu, uint16_t data) {
    uint8_t hi = (uint8_t)(data >> 8);
    uint8_t lo = (uint8_t)(data & 0xff);
    stack_push(cpu, hi);
    stack_push(cpu, lo);
}

uint16_t stack_pop_u16(CPU* cpu) {
    uint16_t lo = (uint16_t)stack_pop(cpu);
    uint16_t hi = (uint16_t)stack_pop(cpu);
    return hi << 8 | lo;
}

void asl_accumulator(CPU* cpu) {
    uint8_t data = cpu->register_a;
    if ((data >> 7) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    set_register_a(cpu, data);
}

uint8_t asl(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    if ((data >> 7) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    mem_write(cpu, addr, data);
    update_zero_and_negative_flags(cpu, data);
    return data;
}

void lsr_accumulator(CPU* cpu) {
    uint8_t data = cpu->register_a;
    if ((data & 1) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    set_register_a(cpu, data);
}

uint8_t lsr(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    if ((data & 1) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    mem_write(cpu, addr, data);
    update_zero_and_negative_flags(cpu, data);
    return data;
}

uint8_t rol(CPU* cpu, enum AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    bool old_carry = contains(cpu->status, CARRY);
    if ((data >> 7) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    if (old_carry) {
        data = data | 1;
    }
    mem_write(cpu, addr, data);
    update_zero_and_negative_flags(cpu, data);
    return data;
}

void rol_accumulator(CPU* cpu, enum AddressingMode mode) {
    uint8_t data = cpu->register_a;
    bool old_carry = contains(cpu->status, CARRY);
    if ((data >> 7) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data << 1;
    if (old_carry) {
        data = data | 1;
    }
    set_register_a(cpu, data);
}

//------ROR-------- (line 410 in the github file)

void run(CPU *cpu) {
    while (1)
    {
        // FETCH
        
        uint8_t code = mem_read(cpu, cpu->program_counter);
        cpu->program_counter += 1;
        uint16_t program_counter_state = cpu->program_counter;
        OpCode op_code = opcode_to_index(code);

        // EXECUTE

        switch (code) {
            case 0xa9:
            case 0xa5:
            case 0xb5:
            case 0xad:
            case 0xbd:
            case 0xb9:
            case 0xa1:
            case 0xb1: {
                lda(cpu, op_code.mode);
                break;
            }

            case 0x85:
            case 0x95:
            case 0x8d:
            case 0x9d:
            case 0x99:
            case 0x81:
            case 0x91: { 
                sta(cpu, op_code.mode);
                break;
            }

            case 0xaa: {
                tax(cpu);
                break;
            }
            case 0xe8:{
                inx(cpu);
                break;
            }
            case 0x00: return;
        }
        
        if (program_counter_state == cpu->program_counter) {
            cpu->program_counter += (uint16_t)(op_code.len - 1);
        } 
    }
}
