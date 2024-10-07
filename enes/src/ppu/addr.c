#include "registers.h"

void init_addr_reg(AddrRegister* addr_reg) {
    addr_reg->value[0] = 0;
    addr_reg->value[1] = 0;
    addr_reg->hi_ptr = true;
}

void set_addr_reg(AddrRegister* addr_reg, uint16_t data) {
    addr_reg->value[0] = (uint8_t)(data >> 8);
    addr_reg->value[1] = (uint8_t)(data & 0xff);
}

uint16_t get_addr_reg(AddrRegister* addr_reg) {
    return (uint16_t)(addr_reg->value[0] << 8) | (uint16_t)addr_reg->value[1];
}

void update_addr_reg(AddrRegister* addr_reg, uint8_t data) {
    if (addr_reg->hi_ptr) {
        addr_reg->value[0] = data;
    } else {
        addr_reg->value[1] = data;
    }

    if (get_addr_reg(addr_reg) > 0x3fff) {
        set_addr_reg(addr_reg, get_addr_reg(addr_reg) & 0b11111111111111);
    }

    addr_reg->hi_ptr = !addr_reg->hi_ptr;
}

void increment_addr_reg(AddrRegister* addr_reg, uint8_t inc) {
    uint8_t lo = addr_reg->value[1];
    addr_reg->value[1] = addr_reg->value[1] + inc;
    if (lo > addr_reg->value[1]) {
        addr_reg->value[0] = addr_reg->value[0] + 1;
    }
    if (get_addr_reg(addr_reg) > 0x3fff) {
        set_addr_reg(addr_reg, get_addr_reg(addr_reg) & 0b11111111111111);
    }
}

void reset_latch_addr_reg(AddrRegister* addr_reg) {
    addr_reg->hi_ptr = true;
}
