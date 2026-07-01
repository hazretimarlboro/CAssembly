#include "../include/Instructions.h"
#include "../include/Tools.h"
#include <stdio.h>

uint32_t fetch_32(uint16_t* PC)
{

    uint16_t pc = *PC;

    if (pc + 3 >= MEMORY_SIZE) {
        CPU.running = 0;
        printf("Segmentation fault (fetch_32 out of bounds)\n");
        return 0;
    }

    uint32_t value =
    (uint32_t)Memory[pc] |
    ((uint32_t)Memory[pc + 1] << 8) |
    ((uint32_t)Memory[pc + 2] << 16) |
    ((uint32_t)Memory[pc + 3] << 24);

    *PC += 4;

    return value;
}

void write_u32(uint16_t addr, uint32_t value)
{
    uint32_t bits = *(uint32_t*)&value;

    Memory[addr + 0] = (uint8_t)(bits & 0xFF);
    Memory[addr + 1] = (uint8_t)((bits >> 8) & 0xFF);
    Memory[addr + 2] = (uint8_t)((bits >> 16) & 0xFF);
    Memory[addr + 3] = (uint8_t)((bits >> 24) & 0xFF);
}
