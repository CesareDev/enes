#ifndef OP_CODE_H
#define OP_CODE_H

#include <stdint.h>

enum AddressingMode {
    Immediate,
    ZeroPage,
    ZeroPage_X,
    ZeroPage_Y,
    Absolute,
    Absolute_X,
    Absolute_Y,
    Indirect_X,
    Indirect_Y,
    NoneAddressing
};

typedef struct {
    uint8_t code;
    char mnemonic[3];
    uint8_t len;
    uint8_t cycles;
    enum AddressingMode mode;
} OpCode;

extern OpCode cpu_op_codes[18];

OpCode opcode_to_index(uint8_t);

#endif // !OP_CODE_H