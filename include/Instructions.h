#ifndef VIMINC_H
#define VIMINC_H

typedef struct StackElement StackElement;

typedef enum ErrorCodes
{
    SUCCESS,
    ALLOCATION_ERROR,
    NULL_POINTER_EXCEPTION,
    DIVISION_BY_ZERO
};

typedef struct StackElement
{
    float value;
    StackElement* sibling;
} StackElement;

typedef struct Register
{
    int ID;
    char* name;
    float value;
} Register;

StackElement* currentTop;
Register rax,rbx,rcx,rdx;

int PUSH(int value);
int POP(void);
int HLT(void);
int MOV(Register* reg, float val);
int ADD(Register* reg);
int MUL(Register* reg);
int DIV(Register* reg);
int MOD(Register* reg);
int LOG(Register* reg);

#endif