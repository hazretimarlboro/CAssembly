#include "../include/Instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/Errors.h"
#include "../include/Tools.h"

Register rax,rbx,rcx,rdx;
struct CPUState CPU;
uint16_t StackPointer;
uint8_t Memory[MEMORY_SIZE];

int init(void)
{
    StackPointer = STACK_TOP; // 0xffff
    CPU.running = 1;
    CPU.flagValid = 0;
    CPU.condition = 0;
    rax.ID = 0;
    rbx.ID = 1;
    rcx.ID = 2;
    rdx.ID = 3;
    rax.value = 0;
    rbx.value = 0;
    rcx.value = 0;
    rdx.value = 0;
    rax.name = "rax";
    rbx.name = "rbx";
    rcx.name = "rcx";
    rdx.name = "rdx";
    return SUCCESS;
}


/*
    We are using a downward growing stack so POP should increase the pointer while PUSH decreases it
*/
int POP(Register* reg)
{
    //check if the stack has values in it
    if(StackPointer == STACK_TOP)
        return STACK_UNDERFLOW;
    
    //move the value into the register and increase stack pointer to restore SP to previous stack frame position
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value = fetch_32(&StackPointer);

    return SUCCESS;
}

int HLT(void)
{
    CPU.running = 0;
    return SUCCESS;
}

int PUSH(uint32_t value)
{
    //check if the stack is full
    if(StackPointer <= STACK_BOTTOM)
        return STACK_OVERFLOW;

    //move value into stack and decrease stack pointer
    StackPointer -= 4;
    write_u32(StackPointer, value);

    return SUCCESS;
}

int MOV_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value = val;

    return SUCCESS;
}

int MOV_REG(Register* reg1, Register* reg2)
{
    //moves the value in one register into another
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;

    reg1->value = reg2->value;
    return SUCCESS;
}

int ADD_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value += val;

    return SUCCESS;
}

int ADD_REG(Register* reg1, Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;
    
    reg1->value += reg2->value;

    return SUCCESS;
}

int MUL_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;
    
    reg->value *= val;

    return SUCCESS;
}

int MUL_REG(Register* reg1, Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;

    reg1->value *= reg2->value;

    return SUCCESS;
}

int DIV_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;
    if(val == 0)
        return DIVISION_BY_ZERO;
    
    reg->value /= val;
    return SUCCESS;
}

int DIV_REG(Register* reg1, Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;
    if(reg2->value == 0)
        return DIVISION_BY_ZERO;
    
    reg1->value /= reg2->value;
    return SUCCESS;
}

int MOD_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;
    if(val == 0)
        return DIVISION_BY_ZERO;
    
    reg->value = reg->value % val;

    return SUCCESS;
}

int MOD_REG(Register* reg1 ,Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;
    if(reg2->value == 0)
        return DIVISION_BY_ZERO;

    reg1->value = reg1->value % reg2->value;
    return SUCCESS;
}

int LOG(Register* reg)
{
    //prints the name of the register and its value
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    // Registers are just 32 bits with no built-in signedness, but MVN exists
    // specifically to produce negative values, so print them as signed
    // decimal (two's complement) rather than always as unsigned.
    printf("%s: %d\n",reg->name,(int32_t)reg->value);

    return SUCCESS;
}

int MVN(Register* reg)
{
    //turns the value in the register into signed
    if (!reg) return NULL_POINTER_EXCEPTION;

    int32_t signed_val = (int32_t)reg->value;
    signed_val = -signed_val;

    reg->value = (uint32_t)signed_val;

    return SUCCESS;
}

int CMP_REG(Register* reg1, Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;
    
    CPU.flagValid = 1;
    CPU.condition = (reg1->value == reg2->value);
    return SUCCESS;
}

int CMP_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    CPU.flagValid = 1;
    CPU.condition = (reg->value == val);
    return SUCCESS;
}
