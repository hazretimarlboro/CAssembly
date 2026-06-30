#include "../include/Instructions.h"
#include <stdio.h>
#include "../include/Tools.h"
#include "../include/Errors.h"

uint16_t PC;

void load_binary(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        perror("Failed to open binary file");
        return;
    }

    size_t bytesRead = fread(Memory, 1, 0x7FFF, f);

    fclose(f);
}


Register* getReg(uint8_t id)
{
    switch(id)
    {
        case 0x00:
            return &rax;
            break;
        case 0x01:
            return &rbx;
            break;
        case 0x02:
            return &rcx;
            break;
        case 0x03:
            return &rdx;
            break;
        default:
            return NULL;
    }
}

int main(int argc, char** argv)
{

    init();

    PC=0;
    
    load_binary(argv[1]);

    while(CPU.running)
    {
        
        if (PC >= MAX_PROGRAM_SIZE)
        {
            CPU.running = 0;
            printf("Segmentation fault: PC out of program bounds at %u\n", PC);
            return 1;
        }

        uint8_t opcode = Memory[PC++];
        switch(opcode)
        {
            case 0x01: {
                // POP
                Register* reg = getReg(Memory[PC]);
                PC++;
                int status = POP(reg);
                if(status == STACK_UNDERFLOW)
                {
                    printf("StackUnderflow at instruction POP.");
                    return 1;
                }
                else if (status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction POP.");
                    return 1;
                }
                break;
            }

            case 0x02: {
                // PUSH
                uint32_t val = fetch_32(&PC);
                int status = PUSH(val);
                if(status == STACK_OVERFLOW)
                {
                    printf("StackOverflow at instruction PUSH.");
                }
                break;
            }

            case 0x03: {
                // MOV_IMM
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val =fetch_32(&PC);
                int status =MOV_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MOV.");
                    return 1;
                }
                
                break;
            }

            case 0x04: {
                // MOV_REG
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = MOV_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MOV.");
                    return 1;
                }

                break;
            }

            case 0x05: {
                //ADD_IMM
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = ADD_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction ADD.");
                    return 1;
                }
                break;
            }

            case 0x06: {
                //ADD_REG
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = ADD_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction ADD.");
                    return 1;
                }
                break;
            }

            case 0x07: {
                // MUL_IMM
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = MUL_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MUL.");
                    return 1;
                }
                break;
            }

            case 0x08: {
                //MUL_REG
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = MUL_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MUL.");
                    return 1;
                }
                break;
            }

            case 0x09: {
                //DIV_IMM
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = DIV_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction DIV.");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("DivisionByZero at instruction DIV");
                    return 1;
                }
                break;
            }

            case 0x0a: {
                //DIV_REG
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = DIV_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction DIV.");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("DivisionByZero at instruction DIV");
                    return 1;
                }
                break;
            }

            case 0x0b: {
                //MOD_IMM
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = MOD_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MOD.");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("DivisionByZero at instruction MOD");
                    return 1;
                }
                break;
            }

            case 0x0c: {
                //MOD_REG
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = MOD_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MOD.");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("DivisionByZero at instruction MOD");
                    return 1;
                }
                break;
            }

            case 0x0d: {
                //LOG
                Register* regleft = getReg(Memory[PC]);
                PC++;
                int status = LOG(regleft);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction LOG.");
                    return 1;
                }

                break;
            }

            case 0x0e: {
                //MVN
                Register* regleft = getReg(Memory[PC]);
                PC++;
                int status = MVN(regleft);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("NullPointerException at instruction MVN.");
                    return 1;
                }

                break;
            }

            case 0xff: {
                //HLT
                HLT();
                return 1;
            }

            default:
                printf("Unknown opcode 0x%02X at PC=%u\n", opcode, PC - 1);
                return 1;
        }

    }

    return 0;
}