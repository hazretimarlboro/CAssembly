#include "../include/Instructions.h"
#include <string.h>

Register register_from_name(char* name)
{
    if(strcmp(name, "rax") == 0)
        return rax;
    else if(strcmp(name, "rbx") == 0)
        return rbx;
    else if(strcmp(name, "rcx")==0)
        return rcx;
    else if(strcmp(name, "rdx")==0)
        return rdx;
    
    Register reg;
    reg.ID = -1;
        return reg;
}