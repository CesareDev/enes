#ifndef OP_CODE_H
#define OP_CODE_H

#include <stdint.h>

#define OP_MAX_COUNT 256

typedef enum {
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
} AddressingMode;

typedef struct {
    uint8_t code;
    const char* mnemonic;
    uint8_t len;
    uint8_t cycles;
    AddressingMode mode;
} OpCode;

void populate_op_index();
OpCode opcode_to_index(uint8_t code);

#endif // !OP_CODE_H
