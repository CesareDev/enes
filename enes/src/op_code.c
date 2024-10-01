#include "op_code.h"
#include <stdlib.h>

OpCode cpu_op_codes[18] = {
    (OpCode){ 0x00, "BRK", 1, 7, NoneAddressing },
    (OpCode){ 0xAA, "TAX", 1, 2, NoneAddressing },
    (OpCode){ 0xE8, "INX", 1, 2, NoneAddressing },

    (OpCode){ 0xA9, "LDA", 2, 2, Immediate },
    (OpCode){ 0xA5, "LDA", 2, 3, ZeroPage },
    (OpCode){ 0xB5, "LDA", 2, 4, ZeroPage_X },
    (OpCode){ 0xAD, "LDA", 3, 4, Absolute },
    (OpCode){ 0xBD, "LDA", 3, 4 /* +1 if page is crossed */, Absolute_X },
    (OpCode){ 0xB9, "LDA", 3, 4 /* +1 if page is crossed */, Absolute_Y },
    (OpCode){ 0xA1, "LDA", 2, 6, Indirect_X },
    (OpCode){ 0xB1, "LDA", 2, 5 /* +1 if page is crossed */, Indirect_Y },

    (OpCode){ 0x85, "STA", 2, 3, ZeroPage },
    (OpCode){ 0x95, "STA", 2, 4, ZeroPage_X },
    (OpCode){ 0x8D, "STA", 3, 4, Absolute },
    (OpCode){ 0x9D, "STA", 3, 5, Absolute_X },
    (OpCode){ 0x99, "STA", 3, 5, Absolute_Y },
    (OpCode){ 0x81, "STA", 2, 6, Indirect_X },
    (OpCode){ 0x91, "STA", 2, 6, Indirect_Y },
};

OpCode opcode_to_index(uint8_t code)
{
    switch (code) {
        case 0x00: return cpu_op_codes[0];
        case 0xAA: return cpu_op_codes[1];
        case 0xE8: return cpu_op_codes[2];
        case 0xA9: return cpu_op_codes[3];
        case 0xA5: return cpu_op_codes[4];
        case 0xB5: return cpu_op_codes[5];
        case 0xAD: return cpu_op_codes[6];
        case 0xBD: return cpu_op_codes[7];
        case 0xB9: return cpu_op_codes[8];
        case 0xA1: return cpu_op_codes[9];
        case 0xB1: return cpu_op_codes[10];
        case 0x85: return cpu_op_codes[11];
        case 0x95: return cpu_op_codes[12];
        case 0x8D: return cpu_op_codes[13];
        case 0x9D: return cpu_op_codes[14];
        case 0x99: return cpu_op_codes[15];
        case 0x81: return cpu_op_codes[16];
        case 0x91: return cpu_op_codes[17];
    }
    abort();
}
