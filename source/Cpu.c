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
        case 0x00: return &rax;
        case 0x01: return &rbx;
        case 0x02: return &rcx;
        case 0x03: return &rdx;
        default: return NULL;
    }
}
int PCvalid(uint16_t PC)
{
    if (PC >= MAX_PROGRAM_SIZE) {
        CPU.running = 0;
        return 0;
    }

    return 1;
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
            printf("[VM ERROR] Segmentation fault: PC out of program bounds at %u\n", PC);
            return 1;
        }

        uint8_t opcode = Memory[PC++];
        switch(opcode)
        {
            case 0x01: {
                // POP
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (POP).\n",opcode);
                    return 1;
                }
                Register* reg = getReg(Memory[PC]);
                
                PC++;
                int status = POP(reg);
                if(status == STACK_UNDERFLOW)
                {
                    printf("[CPU ERROR] StackUnderflow at instruction POP.\n");
                    return 1;
                }
                else if (status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction POP.\n");
                    return 1;
                }
                break;
            }

            case 0x02: {
                // PUSH
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (PUSH).\n",opcode);
                    return 1;
                }
                uint32_t val = fetch_32(&PC);
                int status = PUSH(val);
                if(status == STACK_OVERFLOW)
                {
                    printf("[CPU ERROR] StackOverflow at instruction PUSH.\n");
                }
                break;
            }

            case 0x03: {
                // MOV_IMM
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MOV).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val =fetch_32(&PC);
                int status =MOV_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MOV.\n");
                    return 1;
                }
                
                break;
            }

            case 0x04: {
                // MOV_REG
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MOV).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = MOV_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MOV.\n");
                    return 1;
                }

                break;
            }

            case 0x05: {
                //ADD_IMM
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (ADD).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = ADD_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction ADD.\n");
                    return 1;
                }
                break;
            }

            case 0x06: {
                //ADD_REG
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (ADD).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = ADD_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction ADD.\n");

                    return 1;
                }
                break;
            }

            case 0x07: {
                // MUL_IMM
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MUL).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = MUL_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MUL.\n");
                    return 1;
                }
                break;
            }

            case 0x08: {
                //MUL_REG
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MUL).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = MUL_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MUL.\n");
                    return 1;
                }
                break;
            }

            case 0x09: {
                //DIV_IMM
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (DIV).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = DIV_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction DIV.\n");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("[CPU ERROR] DivisionByZero at instruction DIV\n");
                    return 1;
                }
                break;
            }

            case 0x0a: {
                //DIV_REG
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (DIV).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = DIV_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction DIV.\n");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("[CPU ERROR] DivisionByZero at instruction DIV.\n");
                    return 1;
                }
                break;
            }

            case 0x0b: {
                //MOD_IMM
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MOD).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                uint32_t val = fetch_32(&PC);
                int status = MOD_IMM(regleft,val);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MOD.\n");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("[CPU ERROR] DivisionByZero at instruction MOD.\n");
                    return 1;
                }
                break;
            }

            case 0x0c: {
                //MOD_REG
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MOD).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                Register* regright = getReg(Memory[PC]);
                PC++;
                int status = MOD_REG(regleft,regright);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MOD.\n");
                    return 1;
                }
                else if(status == DIVISION_BY_ZERO)
                {
                    printf("[CPU ERROR] DivisionByZero at instruction MOD.\n");
                    return 1;
                }
                break;
            }

            case 0x0d: {
                //LOG
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (LOG).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                int status = LOG(regleft);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction LOG.\n");
                    return 1;
                }

                break;
            }

            case 0x0e: {
                //MVN
                if(!PCvalid(PC))
                {
                    printf("[VM ERROR] Segmentation fault\n");
                    printf("opcode=0x%02X (MVN).\n",opcode);
                    return 1;
                }
                Register* regleft = getReg(Memory[PC]);
                PC++;
                int status = MVN(regleft);
                if(status == NULL_POINTER_EXCEPTION)
                {
                    printf("[CPU ERROR] NullPointerException at instruction MVN.\n");
                    return 1;
                }

                break;
            }

            case 0x0f: {
                //JMP
                uint32_t target = fetch_32(&PC);
                if (target >= MAX_PROGRAM_SIZE)
                {
                    printf("[VM ERROR] Invalid jump target\n");
                    return 1;
                }
                PC = (uint16_t)target;
                break;
            }

            case 0xff: {
                //HLT
                HLT();
                return 1;
            }

            default:
                printf("[VM ERROR] Unknown opcode 0x%02X at PC=%u\n", opcode, PC - 1);
                return 1;
        }

    }

    return 0;
}