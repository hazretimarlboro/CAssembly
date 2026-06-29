#include <cjson/cJSON.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/Instructions.h"
#include "../include/Tools.h"
#include "../include/Errors.h"

#define MOV_VAL(dest, val) MOV((dest), (val))
#define MOV_REG(dest, src) MOV((dest), (src)->value)

bool isNumber(char str[]) {
    if (strlen(str) == 0) return false;
    for (int i = 0; i < strlen(str); i++) {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}


char* read_file_to_string(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET); 

    char *buffer = malloc(length + 1);
    if (buffer == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    size_t read_size = fread(buffer, 1, length, file);
    buffer[read_size] = '\0';

    fclose(file);
    return buffer;
}

int main()
{
    char* jsoncontent = read_file_to_string("./json/output.json");
    cJSON* json = cJSON_Parse(jsoncontent);
    if(!json)
    {
        printf("JSON parse error");
        return 1;
    }

    int size = cJSON_GetArraySize(json);

    for (int i = 0; i < size; i++)
    {
        cJSON* inst = cJSON_GetArrayItem(json, i);

        cJSON* op_item = cJSON_GetArrayItem(inst, 0);
        char* op = op_item->valuestring;

        cJSON* args = cJSON_GetArrayItem(inst, 1);
        int argc = cJSON_GetArraySize(args);

        if(strcmp(op, "PUSH") == 0)
        {
            cJSON* argq = cJSON_GetArrayItem(args, 0);
            if(!isNumber(argq->valuestring))
            {
                printf("Provided PUSH value is not a number! Terminating process.");
                return 1;
            }
            PUSH(atoi(argq->valuestring));
        }
        else if(strcmp(op,"POP") == 0)
        {
            int status = POP();

        }
        else if(strcmp(op,"MOV") == 0)
        {
            cJSON* argz = cJSON_GetArrayItem(args,0);//register
            cJSON* argg = cJSON_GetArrayItem(args,1);//register or value
            if(isNumber(argg->valuestring))
            {
                Register reg = register_from_name(argz->valuestring);
                if(reg.ID == -1)
                {
                    printf("Provided MOV register does not exist! Terminating process.");
                    return 1;
                }
                MOV_VAL(&reg,atoi(argg->valuestring));
            }
            else
            {
                Register leftreg = register_from_name(argz->valuestring);
                Register rightreg = register_from_name(argg->valuestring);
                if(leftreg.ID == -1 || rightreg.ID == -1)
                {
                    printf("Provided MOV register does not exist! Terminating process.");
                    return 1;
                }
                MOV_REG(&leftreg, &rightreg);
            }
        }
        else if(strcmp(op,"HLT")==0)
        {
            HLT();
            return 0;
        }
        else if(strcmp(op,"ADD")== 0)
        {
            cJSON* argl = cJSON_GetArrayItem(args,0);
            Register reg = register_from_name(argl->valuestring);
            if(reg.ID == -1)
            {
                printf("Provided ADD value is not a register! Terminating process.");
                return 1;
            }

            ADD(&reg);
            return 0;
        }
        else if(strcmp(op,"MUL")== 0)
        {
            cJSON* argl = cJSON_GetArrayItem(args,0);
            Register reg = register_from_name(argl->valuestring);
            if(reg.ID == -1)
            {
                printf("Provided MUL value is not a register! Terminating process.");
                return 1;
            }

            MUL(&reg);
            return 0;
        }
        else if(strcmp(op,"DIV")== 0)
        {
            cJSON* argl = cJSON_GetArrayItem(args,0);
            Register reg = register_from_name(argl->valuestring);
            if(reg.ID == -1)
            {
                printf("Provided ADD value is not a register! Terminating process.");
                return 1;
            }

            ADD(&reg);
            return 0;
        }
        else if(strcmp(op,"ADD")== 0)
        {
            cJSON* argl = cJSON_GetArrayItem(args,0);
            Register reg = register_from_name(argl->valuestring);
            if(reg.ID == -1)
            {
                printf("Provided ADD value is not a register! Terminating process.");
                return 1;
            }

            ADD(&reg);
            return 0;
        }


        for (int j = 0; j < argc; j++)
        {
            cJSON *arg = cJSON_GetArrayItem(args, j);
        }
    }

    free(jsoncontent);
    cJSON_Delete(json);

    return 0;
}