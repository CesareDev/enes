#include <stdio.h>
#include "cpu.h"

int main(int argc, char* argv[]) {
    CPU cpu;
    uint8_t prog[] = { 0xa9, 0xc0, 0xaa, 0xe8, 0x00 };
    printf("Running\n");
    load_and_run(&cpu, prog, 5);
    printf("Register x: %x\n", cpu.register_x);
    return 0;
}
