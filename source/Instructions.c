#include "../include/Instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/Errors.h"
#include "../include/Tools.h"

Register rax,rbx,rcx,rdx,call,stack;
struct CPUState CPU;
uint8_t Memory[MEMORY_SIZE];

int init(void)
{
    CPU.running = 1;
    CPU.flagValid = 0;
    CPU.less = 0;
    CPU.great = 0;
    CPU.eq = 0;

    rax.ID = 0;
    rbx.ID = 1;
    rcx.ID = 2;
    rdx.ID = 3;
    call.ID = 4;
    stack.ID = 5;

    rax.value = 0;
    rbx.value = 0;
    rcx.value = 0;
    rdx.value = 0;
    call.value = 0;
    stack.value = STACK_TOP; // 0xffff

    rax.name = "rax";
    rbx.name = "rbx";
    rcx.name = "rcx";
    rdx.name = "rdx";
    call.name = "call";
    stack.name = "stack";

    return SUCCESS;
}


/*
    We are using a downward growing stack so POP should increase the pointer while PUSH decreases it
*/
int POP(Register* reg)
{
    //check if the stack has values in it
    if(stack.value == STACK_TOP)
        return STACK_UNDERFLOW;
    
    //move the value into the register and increase stack pointer to restore SP to previous stack frame position
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value = fetch_32((uint16_t*) &(stack.value));

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
    if(stack.value <= STACK_BOTTOM)
        return STACK_OVERFLOW;

    //move value into stack and decrease stack pointer
    stack.value -= 4;
    write_u32(stack.value, value);

    return SUCCESS;
}

int PUSH_REG(Register* reg) {
    if (stack.value <= STACK_BOTTOM) {
        return STACK_OVERFLOW;
    }
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    stack.value -= 4;
    write_u32(stack.value, reg->value);

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

    // Values can be negative (see MVN), so divide as signed two's
    // complement, not as raw uint32_t - otherwise a negative operand
    // gets reinterpreted as a huge positive number and the result is
    // garbage instead of a truncated-toward-zero signed quotient.
    int32_t result = (int32_t)reg->value / (int32_t)val;
    reg->value = (uint32_t)result;
    return SUCCESS;
}

int DIV_REG(Register* reg1, Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;
    if(reg2->value == 0)
        return DIVISION_BY_ZERO;

    int32_t result = (int32_t)reg1->value / (int32_t)reg2->value;
    reg1->value = (uint32_t)result;
    return SUCCESS;
}

int MOD_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;
    if(val == 0)
        return DIVISION_BY_ZERO;

    int32_t result = (int32_t)reg->value % (int32_t)val;
    reg->value = (uint32_t)result;

    return SUCCESS;
}

int MOD_REG(Register* reg1 ,Register* reg2)
{
    if(!reg1 || !reg2)
        return NULL_POINTER_EXCEPTION;
    if(reg2->value == 0)
        return DIVISION_BY_ZERO;

    int32_t result = (int32_t)reg1->value % (int32_t)reg2->value;
    reg1->value = (uint32_t)result;
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

    // Compare as signed 32-bit values (see MVN) - a plain uint32_t
    // compare treats every negative value as larger than every
    // positive one, which breaks JL/JG/JLE/JGE whenever either side
    // is negative.
    int32_t v1 = (int32_t)reg1->value;
    int32_t v2 = (int32_t)reg2->value;

    CPU.flagValid = 1;
    CPU.eq = (v1 == v2);
    CPU.less = (v1 < v2);
    CPU.great = (v1 > v2);
    return SUCCESS;
}

int CMP_IMM(Register* reg, uint32_t val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    int32_t v1 = (int32_t)reg->value;
    int32_t v2 = (int32_t)val;

    CPU.flagValid = 1;
    CPU.eq = (v1 == v2);
    CPU.less = (v1 < v2);
    CPU.great = (v1 > v2);
    return SUCCESS;
}

int LOAD_REG(Register* reg, uint32_t ptr)
{
    if(ptr > DATA_MAX || ptr < DATA_MIN)
        return SEGMENTATION_FAULT;

    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value = fetch_32((uint16_t*) &ptr);
    return SUCCESS;
}

int STORE_PTR_REG(uint32_t lptr, Register* reg)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;
    if(lptr > DATA_MAX || lptr < DATA_MIN)
        return SEGMENTATION_FAULT;

    write_u32(lptr, reg->value);
    return SUCCESS;
}

