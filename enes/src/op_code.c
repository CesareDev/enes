#include "op_code.h"

OpCode cpu_op_codes[18] = {
    (OpCode){ 0x00, "BRK", 1, 7, NoneAddressing },
    (OpCode){ 0xAA, "TAX", 1, 2, NoneAddressing },
    (OpCode){ 0xE8, "INX", 1, 2, NoneAddressing },

    (OpCode){ 0xA9, "LDA", 2, 2, Immediate },
    (OpCode){ 0xA5, "LDA", 2, 3, ZeroPage },
    (OpCode){ 0xB5, "LDA", 2, 4, ZeroPage_X },
    (OpCode){ 0xAD, "LDA", 3, 4, Absolute },
    (OpCode){ 0xBD, "LDA", 3, 4 /* +1 if page is crossed */, Absolute_X },
    (OpCode){ 0xb9, "LDA", 3, 4 /* +1 if page is crossed */, Absolute_Y },
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
