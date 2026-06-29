#include "../include/Instructions.h"
#include <stdio.h>
#include <stdlib.h>
#include "../include/Errors.h"

#define MAX_SIZE 4096


StackElement* currentTop;
Register rax,rbx,rcx,rdx;

int init(void)
{
    currentTop = NULL;
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

int POP(void)
{
    if(!currentTop)
        return NULL_POINTER_EXCEPTION;

    //free the current stack's top while preserving the 2nd element as the new top
    StackElement* sibling = currentTop->sibling;
    free(currentTop);
    currentTop = sibling;
    return SUCCESS;
}

int HLT(void)
{
    //self explanatory
    while(currentTop)
    {
        POP();
    }
    return SUCCESS;
}

int PUSH(float value)
{
    if(!currentTop)
    {
        //if there is no one in the stack, create a new element and make it the stack's top
        StackElement* newElement = malloc(sizeof(StackElement));
        if(!newElement)
            return ALLOCATION_ERROR;
        newElement->value = value;
        newElement->sibling = NULL;
        currentTop = newElement;
        return SUCCESS;
    }
    
    //if there is someone in the stack, create a new element and make the element point to the
    //stack's top, then make the new element the new top
    StackElement* newEl = malloc(sizeof(StackElement));
    if(!newEl)
        return ALLOCATION_ERROR;
    newEl->value = value;
    newEl->sibling = currentTop;
    currentTop = newEl;
    return SUCCESS;
}

int MOV(Register* reg, float val)
{
    if(!reg)
        return NULL_POINTER_EXCEPTION;
    //moves a value into a register
    //works on register-register pattern via the macros
    reg->value = val;

    return SUCCESS;
}

int ADD(Register* reg)
{
    // takes the value from top of the stack and adds it to the register
    if(!reg || !currentTop)
        return NULL_POINTER_EXCEPTION;
    
    reg->value += currentTop->value;
    
    return POP();
}

int MUL(Register* reg)
{
    //takes the value from top of the stack and multiplies it with the register
    //then moves that value to the register
    if(!reg || !currentTop)
        return NULL_POINTER_EXCEPTION;
    
    reg->value *= currentTop->value;

    return POP();
}

int DIV(Register* reg)
{
    //divides the register's value with the value at the top of the stack
    //sets the register value as that value
    if(!reg || !currentTop)
        return NULL_POINTER_EXCEPTION;
    
    if(currentTop->value == 0)
        return DIVISION_BY_ZERO;
    
    reg->value /= currentTop->value;

    return POP();
}

int MOD(Register* reg)
{
    //takes the value from the register, modulos it with the value from the top of the stack
    //and moves it to the register
    if(!reg || !currentTop)
        return NULL_POINTER_EXCEPTION;

    if(currentTop->value == 0)
        return DIVISION_BY_ZERO;
    
    reg->value = (int)reg->value % (int)currentTop->value;

    return POP();
}

int LOG(Register* reg)
{
    //prints the name of the register and its name
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    printf("%s: %f\n",reg->name,reg->value);

    return SUCCESS;
}

int MVN(Register* reg)
{
    //multiplies the register's value by -1
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value = (-1) * reg->value;

    return SUCCESS;
}

int OR(Register* reg)
{
    //performs and OR operation with the integer values of the register and the value at the top of the stack
    //then moves the value into the register
    if(!reg)
        return NULL_POINTER_EXCEPTION;

    reg->value = (int) reg->value | (int) currentTop->value;
    return SUCCESS;
}

int main(int argc, char** argv)
{



    // init();
    // PUSH(12983);
    // PUSH(923);
    // MOV_VAL(&rax,123);
    // MOV_REG(&rbx,&rax);
    // MUL(&rax);
    // LOG(&rax);
    // MOD(&rax);
    // LOG(&rax);
    // PUSH(32);
    // ADD(&rbx);
    // LOG(&rbx);
    // PUSH(23);   
    // DIV(&rbx);
    // MVN(&rbx);
    // LOG(&rbx);
    // HLT();

    return 0;
}