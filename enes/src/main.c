#include <stdio.h>
#include "cpu.h"

int main(int argc, char* argv[]) {
    CPU cpu = { 0, 0, 0 };
    uint8_t prog[] = { 0xa9, 0x00, 0x00 };
    cpu_interpret(&cpu, prog);
    printf("Register a: %d\n", cpu.register_a);
    printf("Status: %d\n", cpu.status);
    return 0;
}
