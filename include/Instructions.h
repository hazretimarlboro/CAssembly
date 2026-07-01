#ifndef VIMINC_H
#define VIMINC_H
#define MEMORY_SIZE 0xFFFF
#define STACK_TOP 0xFFFF
#define STACK_BOTTOM 0xA000
#define MAX_PROGRAM_SIZE 0x7FFF
#include <stdint.h>

typedef struct Register
{
    uint8_t ID;
    char* name;
    uint32_t value;
} Register;

struct CPUState
{
    int running;
    int condition;
    int flagValid;
};

extern uint8_t Memory[MEMORY_SIZE];
/*
    0x0000 - 0x7FFF -> the program
    0x8000 - 0x9FFF → data (variables)
    0xA000 - 0xffff -> Stack
    because this prevents chaos between the stack and the memory layouts.
*/


extern uint16_t StackPointer;
extern Register rax,rbx,rcx,rdx;
extern struct CPUState CPU;

int init(void);

int PUSH(uint32_t value);
int POP(Register* reg);
int HLT(void);
int MOV_IMM(Register* reg, uint32_t val);
int MOV_REG(Register* reg1, Register* reg2);
int ADD_IMM(Register* reg, uint32_t val);
int ADD_REG(Register* reg, Register* reg2);
int MUL_IMM(Register* reg, uint32_t val);
int MUL_REG(Register* reg, Register* reg2);
int DIV_IMM(Register* reg, uint32_t val);
int DIV_REG(Register* reg, Register* reg2);
int MOD_IMM(Register* reg, uint32_t val);
int MOD_REG(Register* reg, Register* reg2);
int CMP_IMM(Register* reg, uint32_t val);
int CMP_REG(Register* reg, Register* reg2);
int LOG(Register* reg);
int MVN(Register* reg);


#endif
