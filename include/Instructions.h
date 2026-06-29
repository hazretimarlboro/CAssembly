#ifndef VIMINC_H
#define VIMINC_H

typedef struct StackElement StackElement;

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

int PUSH(float value);
int POP(void);
int HLT(void);
int MOV(Register* reg, float val);
int ADD(Register* reg);
int MUL(Register* reg);
int DIV(Register* reg);
int MOD(Register* reg);
int LOG(Register* reg);
int MVN(Register* reg);

#endif