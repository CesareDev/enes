#ifndef ROM_H
#define ROM_H

#include <stdbool.h>
#include <stdint.h>

#define NES_TAG_0 0x4e
#define NES_TAG_1 0x45
#define NES_TAG_2 0x53
#define NES_TAG_3 0x1a
#define PRG_ROM_PAGE_SIZE 16384
#define CHR_ROM_PAGE_SIZE 8192

typedef enum {
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN
} Mirroring;

typedef struct {
    uint8_t* prg_rom;
    uint8_t* chr_rom;
    uint8_t mapper;
    Mirroring screen_mirroring;
} Rom;

typedef struct {
    bool valid;
    Rom rom;
} RomResult;

RomResult load_rom(const char* name);
void unload_rom(Rom* rom);

#endif // !ROM_H
