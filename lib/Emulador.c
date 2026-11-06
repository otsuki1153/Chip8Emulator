#include "Emulador.h"
#include <string.h>
#include <stdio.h>

void initEmulator(Emulador * param){
    param->sysCount = START_ADDR;
    param->regI = 0;
    param->posPilha = 0;
    memset(param->memoria, 0,sizeof(param->memoria));
    memset(param->regV, 0,sizeof(param->regV));
    memset(param->pilha, 0,sizeof(param->pilha));
}

void incSysCount(Emulador * param){
    param->sysCount += 2;
}

bool pilhaPush(Emulador * param, uint16_t address){
    if(param->posPilha >= STACK_S){
        //ERROR: STACK OVERFLOW
        return false;
    }

    param->pilha[param->posPilha] = address;
    param->posPilha++;
    return true;
}


uint16_t pilhaPop(Emulador * param, bool * success){
    if(param->posPilha == 0){
        //ERROR: STACK UNDERFLOW
        if(success) *success = false;
        return 0;
    }
    param->posPilha--;
    if(success) *success = true;
    return param->pilha[param->posPilha];
}

void decode(uint16_t opcodeParam, char *retorno){
    uint16_t opcode = opcodeParam;

    uint8_t high_byte = (opcode >> 8);
    uint8_t low_byte = opcode & 0xFF;
    
    uint8_t n1 = (high_byte >> 4) & 0x0F;
    uint8_t n2 =  high_byte  & 0x0F;
    uint8_t n3 = (low_byte >> 4)  & 0x0F;
    uint8_t n4 = low_byte & 0x0F;

    uint16_t NNN = opcode & 0xFFF;

    switch (n1){
        case 0x0:
            if(n2 == 0x0 && n3 == 0xE && n4 == 0){
                strcpy(retorno, "CLS");
            } else if(n2 == 0x0 && n3 == 0xE && n4 == 0xE){
                strcpy(retorno, "RET");
            } else{
                snprintf(retorno, 50, "SYS 0x%03X", NNN);
            }
            break;
        case 0x1:
            snprintf(retorno, 50, "JP 0x%03X", NNN);
            break;
        case 0x2:
            snprintf(retorno, 50, "CALL 0x%03X", NNN);
            break;
        case 0x3:
            snprintf(retorno, 50, "SE V%X, 0x%02X", n2, low_byte);
            break;
        case 0x4:
            snprintf(retorno, 50, "SNE V%X, 0x%02X", n2, low_byte);
            break;
        case 0x5:
            if(n4 == 0x0){
                snprintf(retorno, 50, "SE V%X, V%X",n2, n3);
            } else{
                strcpy(retorno, "????");
            }
            break;
        case 0x6:
            snprintf(retorno, 50, "LD V%X, 0x%02X", n2, low_byte);
            break;
        case 0x7:
            snprintf(retorno, 50, "ADD V%X, 0x%02X", n2, low_byte);
            break;
        case 0x8:
            if(n4 == 0x0){
                snprintf(retorno, 50, "LD V%X, V%X", n2, n3);
            } else if(n4 == 0x1){
                snprintf(retorno, 50, "OR V%X, V%X", n2, n3);
            } else if(n4 == 0x2){
                snprintf(retorno, 50, "AND V%X, V%X", n2, n3);
            } else if(n4 == 0x3){
                snprintf(retorno, 50, "XOR V%X, V%X", n2, n3);
            } else if(n4 == 0x4){
                snprintf(retorno, 50, "ADD V%X, V%X", n2, n3);
            } else if(n4 == 0x5){
                snprintf(retorno, 50, "SUB V%X, V%X", n2, n3);
            } else if(n4 == 0x6){
                snprintf(retorno, 50, "SHR V%X, V%X", n2, n3);
            } else if(n4 == 0x7){
                snprintf(retorno, 50, "SUBN V%X, V%X", n2, n3);
            } else if(n4 == 0xE){
                snprintf(retorno, 50, "SHL V%X, V%X", n2, n3);
            }else{
                strcpy(retorno, "????");
            }
            break;
        case 0x9:
            if(n4 == 0x0){
                snprintf(retorno, 50, "SNE V%X, V%X", n2, n3);
            } else{
                strcpy(retorno, "????");
            }
            break;
        case 0xA:
            snprintf(retorno, 50, "LD I, 0x%03X", NNN);
            break;
        case 0xB:
            snprintf(retorno, 50, "JP V0, 0x%03X",NNN);
            break;
        case 0xC:
            snprintf(retorno, 50, "RND V%X, 0x%02X", n2, low_byte);
            break;
        case 0xD:
            snprintf(retorno, 50, "DRW V%X, V%X, 0x%X", n2,n3, n4);
            break;
        case 0xE:
            if(n3 == 0x9 && n4 == 0xE){
                snprintf(retorno, 50, "SKP V%X",n2);
            } else if(n3 == 0xA && n4 == 0x1){
                snprintf(retorno, 50, "SKNP V%X",n2);
            } else{
                strcpy(retorno, "????");
            }
            break;
        case 0xF:
            if(n3 == 0x0){
                if(n4 == 0x7){
                    snprintf(retorno, 50, "LD V%X, DT",n2);
                } else if(n4 == 0xA){
                    snprintf(retorno, 50, "LD V%X, K",n2);
                } else{
                    strcpy(retorno, "????");
                }
            } else if (n3 == 0x1){
                if(n4 == 0x5){
                    snprintf(retorno, 50, "LD DT, V%X",n2);
                } else if(n4 == 0x8){
                    snprintf(retorno, 50, "LD ST, V%X",n2);
                } else if(n4 == 0xE){
                    snprintf(retorno, 50, "ADD I, V%X",n2);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(n3 == 0x2){
                if(n4 == 0x9){
                    snprintf(retorno, 50, "LD F, V%X",n2);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(n3 == 0x3){
                if(n4 == 0x3){
                    snprintf(retorno, 50, "LD B, V%X",n2);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(n3 == 0x5){
                if(n4 == 0x5){
                    snprintf(retorno, 50, "LD [I], V%X",n2);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(n3 == 0x6){
                if(n4 == 0x5){
                    snprintf(retorno, 50, "LD V%X, [I]",n2);
                } else{
                    strcpy(retorno, "????");
                }
            } else{
                strcpy(retorno, "????");
            }
            
            break;
        default:
            strcpy(retorno, "????");
            break;
    }
    
}
