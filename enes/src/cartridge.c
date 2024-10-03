#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cartridge.h"

RomResult load_rom(const char* name) {
    FILE* game = fopen(name, "r");
    if (!game) {
        return (RomResult){ false, (Rom){ 0, 0, 0, 0 } };
    }
    fseek(game, 0, SEEK_END);
    uint64_t prog_len = ftell(game);
    rewind(game);
    uint8_t* raw = (uint8_t*)malloc(prog_len * sizeof(uint8_t));
    fread(raw, prog_len, 1, game);

    if (raw[0] != NES_TAG_0 || raw[1] != NES_TAG_1 || raw[2] != NES_TAG_2 || raw[3] != NES_TAG_3) {
        free(raw);
        fclose(game);
        return (RomResult){ false, (Rom){ 0, 0, 0, 0 } };
    }

    uint8_t mapper = (raw[7] & 0b11110000) | (raw[6] >> 4);
    uint8_t ines_ver = (raw[7] >> 2) & 0b11;

    if (ines_ver != 0) {
        free(raw);
        fclose(game);
        return (RomResult){ false, (Rom){ 0, 0, 0, 0 } };
    }

    uint8_t four_screen = (raw[6] & 0b1000) != 0;
    uint8_t vertical_mirroring = (raw[6] & 0b1) != 0;
    Mirroring screen_mirroring;
    if (four_screen) screen_mirroring = FOUR_SCREEN;
    if (!four_screen && vertical_mirroring) screen_mirroring = VERTICAL;
    if (!four_screen && !vertical_mirroring) screen_mirroring = HORIZONTAL;

    uint64_t prg_rom_size = (uint64_t)raw[4] * PRG_ROM_PAGE_SIZE;
    uint64_t chr_rom_size = (uint64_t)raw[4] * CHR_ROM_PAGE_SIZE;

    uint8_t skip_trainer = (raw[6] & 0b100) != 0;

    uint64_t prg_rom_start = 16;
    if (skip_trainer) prg_rom_start += 512;
    uint64_t chr_rom_start = prg_rom_start + prg_rom_size;

    RomResult res;
    res.valid = true;
    res.rom.mapper = mapper;
    res.rom.screen_mirroring = screen_mirroring;

    res.rom.prg_rom = (uint8_t*)malloc(prg_rom_size);
    res.rom.chr_rom = (uint8_t*)malloc(chr_rom_start);

    memcpy(res.rom.prg_rom, raw + prg_rom_start, prg_rom_size);
    memcpy(res.rom.chr_rom, raw + chr_rom_start, chr_rom_size);

    free(raw);
    fclose(game);
    
    return res;
}

void unload_rom(Rom *rom) {
    free(rom->prg_rom);
    free(rom->chr_rom);
}
