#include "cpu.h"
#include "op_code.h"

#include <stdlib.h>

void insert(CpuFlag* status, Flag flag) {
    *status = *status | flag;
}

void remove(CpuFlag* status, Flag flag) {
    *status = *status & (~flag);
}

// ChatGPT -> NOT SURE!
void set(CpuFlag* status, Flag flag, bool condition) {
    if (condition) {
        *status = *status | flag;
    } else {
        *status = *status & ~flag;
    }
}

bool contains(CpuFlag status, Flag flag) {
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

uint8_t mem_read(CPU* cpu, uint16_t addr) {
    return bus_mem_read(cpu->bus, addr);
}

void mem_write(CPU* cpu, uint16_t addr, uint8_t data) {
    bus_mem_write(cpu->bus, addr, data);
}

uint16_t mem_read_uint16(CPU* cpu, uint16_t pos) {
    return bus_mem_read_u16(cpu->bus, pos);
}

void mem_write_uint16(CPU* cpu, uint16_t pos, uint16_t data) {
    bus_mem_write_u16(cpu->bus, pos, data);
}

uint16_t get_operand_address(CPU* cpu, AddressingMode mode) {
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

void ldy(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_y = value;
    update_zero_and_negative_flags(cpu, cpu->register_y);
}

void ldx(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_x = value;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

void lda(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t value = mem_read(cpu, addr);
    cpu->register_a = value;
    update_zero_and_negative_flags(cpu, cpu->register_a);
}

void sta(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    mem_write(cpu, addr, cpu->register_a);
}

void set_register_a(CPU* cpu, uint8_t value) {
    cpu->register_a = value;
    update_zero_and_negative_flags(cpu, cpu->register_a);
}

void and(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    set_register_a(cpu, data & cpu->register_a);
}

void eor(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    set_register_a(cpu, data ^ cpu->register_a);
}

void ora(CPU* cpu, AddressingMode mode) {
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

void load_and_reset(CPU *cpu, uint8_t *program, uint16_t size) {
    load(cpu, program, size);
    reset(cpu);
}

void init_cpu(CPU* cpu, Bus* bus, Rom* rom) {
    cpu->bus = bus;
    bus->rom = rom;
    populate_op_index();
    init_vram(bus);
    reset(cpu);
}

void load(CPU *cpu, uint8_t *program, uint16_t size) {
    populate_op_index();
    init_vram(cpu->bus);
    for (uint16_t i = 0; i < size; i++) {
        mem_write(cpu, 0x0600 + i, program[i]);
    }
    mem_write_uint16(cpu, 0xfffc, 0x0600);
}

void reset(CPU* cpu) {
    cpu->register_a = 0;
    cpu->register_x = 0;
    cpu->register_y = 0;
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
    if (((data ^ result) & (result ^ cpu->register_a) & 0x80) != 0) {
        insert(&cpu->status, OVERFLOW);
    } else {
        remove(&cpu->status, OVERFLOW);
    }
    set_register_a(cpu, result);
}

void sbc(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    // ChatGPT tell this NOT SURE!
    int8_t tmp_resul = -(data) - 1;
    uint8_t result = (uint8_t)tmp_resul;
    add_to_register_a(cpu, result);
}

void adc(CPU* cpu, AddressingMode mode) {
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

uint8_t asl(CPU* cpu, AddressingMode mode) {
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

uint8_t lsr(CPU* cpu, AddressingMode mode) {
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

uint8_t rol(CPU* cpu, AddressingMode mode) {
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

void rol_accumulator(CPU* cpu) {
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

uint8_t ror(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    bool old_carry = contains(cpu->status, CARRY);
    if ((data & 1) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    if (old_carry) {
        data = data | 0b10000000;
    }
    mem_write(cpu, addr, data);
    update_zero_and_negative_flags(cpu, data);
    return data;
}

void ror_accumulator(CPU* cpu) {
    uint8_t data = cpu->register_a;
    bool old_carry = contains(cpu->status, CARRY);
    if ((data & 1) == 1) {
        set_carry_flag(cpu);
    } else {
        clear_carry_flag(cpu);
    }
    data = data >> 1;
    if (old_carry) {
        data = data | 0b10000000;
    }
    set_register_a(cpu, data);
}

uint8_t inc(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    data++;
    mem_write(cpu, addr, data);
    update_zero_and_negative_flags(cpu, data);
    return data;
}

void dey(CPU* cpu) {
    cpu->register_y--;
    update_zero_and_negative_flags(cpu, cpu->register_y);
}

void dex(CPU* cpu) {
    cpu->register_x--;
    update_zero_and_negative_flags(cpu, cpu->register_x);
}

uint8_t dec(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    data--;
    mem_write(cpu, addr, data);
    update_zero_and_negative_flags(cpu, data);
    return data;
}

void pla(CPU* cpu) {
    uint8_t data = stack_pop(cpu);
    set_register_a(cpu, data);
}

void plp(CPU* cpu) {
    cpu->status = stack_pop(cpu);
    remove(&cpu->status, BREAK);
    insert(&cpu->status, BREAK2);
}

void php(CPU* cpu) {
    CpuFlag flags = cpu->status;
    insert(&flags, BREAK);
    insert(&flags, BREAK2);
    stack_push(cpu, flags);
}

void bit(CPU* cpu, AddressingMode mode) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    uint8_t and = cpu->register_a & data;
    if (and == 0) {
        insert(&cpu->status, ZERO);
    } else {
        remove(&cpu->status, ZERO);
    }
    set(&cpu->status, NEGATIV, (data & 0b10000000) > 0);
    set(&cpu->status, OVERFLOW, (data & 0b01000000) > 0);
}

void compare(CPU* cpu, AddressingMode mode, uint8_t compare_with) {
    uint16_t addr = get_operand_address(cpu, mode);
    uint8_t data = mem_read(cpu, addr);
    if (data <= compare_with) {
        insert(&cpu->status, CARRY);
    } else {
        remove(&cpu->status, CARRY);
    }
    set(&cpu->status, NEGATIV, (data & 0b10000000) > 0);
    set(&cpu->status, OVERFLOW, (data & 0b01000000) > 0);
    uint8_t result = compare_with - data;
    update_zero_and_negative_flags(cpu, result);
}

void branch(CPU* cpu, bool condition) {
    if (condition) {
        int8_t jump = mem_read(cpu, cpu->program_counter);
        uint16_t jump_addr = cpu->program_counter + 1 + (uint16_t)jump;
        cpu->program_counter = jump_addr;
    }
}

bool cycle(CPU *cpu) {
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
        case 0xb1: lda(cpu, op_code.mode); break;

        case 0xaa: tax(cpu); break;

        case 0xe8: inx(cpu); break;

        case 0x00: return false;

        case 0xd8: remove(&cpu->status, DECIMAL_MODE); break;
        case 0x58: remove(&cpu->status, INTERRUPT_DISABLE); break;
        case 0xb8: remove(&cpu->status, OVERFLOW); break;

        case 0x18: clear_carry_flag(cpu); break;

        case 0x38: set_carry_flag(cpu); break;

        case 0x78: insert(&cpu->status, INTERRUPT_DISABLE); break;
        case 0xf8: insert(&cpu->status, DECIMAL_MODE); break;

        case 0x48: stack_push(cpu, cpu->register_a); break;

        case 0x68: pla(cpu); break;

        case 0x08: php(cpu); break;

        case 0x28: plp(cpu); break;

        case 0x69:
        case 0x65:
        case 0x75:
        case 0x6d:
        case 0x7d:
        case 0x79:
        case 0x61:
        case 0x71: adc(cpu, op_code.mode); break;

        case 0xe9:
        case 0xe5:
        case 0xf5:
        case 0xed:
        case 0xfd:
        case 0xf9:
        case 0xe1:
        case 0xf1: sbc(cpu, op_code.mode); break;

        case 0x29:
        case 0x25:
        case 0x35:
        case 0x2d:
        case 0x3d:
        case 0x39:
        case 0x21:
        case 0x31: and(cpu, op_code.mode); break;

        case 0x49:
        case 0x45:
        case 0x55:
        case 0x4d:
        case 0x5d:
        case 0x59:
        case 0x41:
        case 0x51: eor(cpu, op_code.mode); break;

        case 0x09:
        case 0x05:
        case 0x15:
        case 0x0d:
        case 0x1d:
        case 0x19:
        case 0x01:
        case 0x11: ora(cpu, op_code.mode); break;

        case 0x4a: lsr_accumulator(cpu); break;

        case 0x46:
        case 0x56:
        case 0x4e:
        case 0x5e: lsr(cpu, op_code.mode); break;

        case 0x0a: asl_accumulator(cpu); break;

        case 0x06:
        case 0x16:
        case 0x0e:
        case 0x1e: asl(cpu, op_code.mode); break;

        case 0x2a: rol_accumulator(cpu); break;

        case 0x26:
        case 0x36:
        case 0x2e:
        case 0x3e: rol(cpu, op_code.mode); break;

        case 0x6a: ror_accumulator(cpu);  break;

        case 0x66:
        case 0x76:
        case 0x6e:
        case 0x7e: ror(cpu, op_code.mode); break;

        case 0xe6:
        case 0xf6:
        case 0xee:
        case 0xfe: inc(cpu, op_code.mode); break;

        case 0xc8: iny(cpu); break;

        case 0xc6:
        case 0xd6:
        case 0xce:
        case 0xde: dec(cpu, op_code.mode); break;

        case 0xca: dex(cpu); break;

        case 0x88: dey(cpu); break;

        case 0xc9:
        case 0xc5:
        case 0xd5:
        case 0xcd:
        case 0xdd:
        case 0xd9:
        case 0xc1:
        case 0xd1: compare(cpu, op_code.mode, cpu->register_a); break;

        case 0xc0:
        case 0xc4:
        case 0xcc: compare(cpu, op_code.mode, cpu->register_y); break;

        case 0xe0:
        case 0xe4:
        case 0xec: compare(cpu, op_code.mode, cpu->register_x); break;

        case 0x4c: {
            uint16_t mem_address = mem_read_uint16(cpu, cpu->program_counter);
            cpu->program_counter =  mem_address;
            break;
        }

        case 0x6c: {
            uint16_t mem_address = mem_read_uint16(cpu, cpu->program_counter);
            uint16_t indirect_ref = 0;
            if ((mem_address & 0x00ff) == 0x00ff) {
                uint8_t lo = mem_read(cpu, mem_address);
                uint8_t hi = mem_read(cpu, mem_address & 0xff00);
                indirect_ref = (uint16_t)hi << 8 | (uint16_t)lo;
            } else {
                indirect_ref = mem_read_uint16(cpu, mem_address);
            }
            cpu->program_counter = indirect_ref;
            break;
        }

        case 0x20: {
            stack_push_u16(cpu, cpu->program_counter + 2 - 1);
            uint16_t target_address = mem_read_uint16(cpu, cpu->program_counter);
            cpu->program_counter = target_address;
            break;
        }

        case 0x60: {
            cpu->program_counter = stack_pop_u16(cpu) + 1;
            break;
        }

        case 0x40: {
            cpu->status = stack_pop(cpu);
            remove(&cpu->status, BREAK);
            insert(&cpu->status, BREAK2);
            cpu->program_counter = stack_pop_u16(cpu);
            break;
        }

        case 0xd0: branch(cpu, !contains(cpu->status, ZERO)); break;
        case 0x70: branch(cpu, contains(cpu->status, OVERFLOW)); break;
        case 0x50: branch(cpu, !contains(cpu->status, OVERFLOW)); break;
        case 0x10: branch(cpu, !contains(cpu->status, NEGATIV)); break;
        case 0x30: branch(cpu, contains(cpu->status, NEGATIV)); break;
        case 0xf0: branch(cpu, contains(cpu->status, ZERO)); break;
        case 0xb0: branch(cpu, contains(cpu->status, CARRY)); break;
        case 0x90: branch(cpu, !contains(cpu->status, CARRY)); break;

        case 0x24:
        case 0x2c: bit(cpu, op_code.mode); break;

        case 0x85:
        case 0x95:
        case 0x8d:
        case 0x9d:
        case 0x99:
        case 0x81:
        case 0x91: sta(cpu, op_code.mode); break;

        case 0x86:
        case 0x96:
        case 0x8e: {
            uint16_t addr = get_operand_address(cpu, op_code.mode);
            mem_write(cpu, addr, cpu->register_x);
            break;
        }

        case 0x84:
        case 0x94:
        case 0x8c: {
            uint16_t addr = get_operand_address(cpu, op_code.mode);
            mem_write(cpu, addr, cpu->register_y);
            break;
        }

        case 0xa2:
        case 0xa6:
        case 0xb6:
        case 0xae:
        case 0xbe: ldx(cpu, op_code.mode); break;

        case 0xa0:
        case 0xa4:
        case 0xb4:
        case 0xac:
        case 0xbc: ldy(cpu, op_code.mode); break;

        case 0xea: break;

        case 0xa8: {
            cpu->register_y = cpu->register_a;
            update_zero_and_negative_flags(cpu, cpu->register_y);
            break;
        }

        case 0xba: {
            cpu->register_x = cpu->stack_pointer;
            update_zero_and_negative_flags(cpu, cpu->register_x);
            break;
        }

        case 0x8a: {
            cpu->register_a = cpu->register_x;
            update_zero_and_negative_flags(cpu, cpu->register_a);
            break;
        }

        case 0x9a: {
            cpu->stack_pointer = cpu->register_x;
            break;
        }

        case 0x98: {
            cpu->register_a = cpu->register_y;
            update_zero_and_negative_flags(cpu, cpu->register_a);
            break;
        }
    }

    if (program_counter_state == cpu->program_counter) {
        cpu->program_counter += (uint16_t)(op_code.len - 1);
    }

    return true;
}
