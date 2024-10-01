#include <stdio.h>
#include "cpu.h"

int main(int argc, char* argv[]) {
    CPU cpu;
    uint8_t prog[] = { 0xa9, 0xff, 0xaa, 0xe8, 0xe8, 0x00 };
    load_and_run(&cpu, prog, 6);
    printf("Register x: %x\n", cpu.register_x);
    return 0;
}
