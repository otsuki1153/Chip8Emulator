#include "Emulador.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void cleanTerminal(){
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void initEmulator(Emulador * param){
    param->sysCount = START_ADDR;
    param->regI = 0;
    param->posPilha = 0;
    memset(param->memoria, 0,sizeof(param->memoria));
    memset(param->regV, 0,sizeof(param->regV));
    memset(param->pilha, 0,sizeof(param->pilha));
    memset(param->tela, false, sizeof(param->tela));
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


uint16_t pilhaPop(Emulador * param, bool * resultado){
    if(param->posPilha == 0){
        //ERROR: STACK UNDERFLOW
        *resultado = false;
        return 0;
    }
    param->posPilha--;
    *resultado = true;
    return param->pilha[param->posPilha];
}

bool fetch(Emulador * param, uint16_t *opcodeReturn){
    if(param->sysCount >= MEMORY_S - 1) return false;
    uint16_t opcode = (param->memoria[param->sysCount] << 8) | param->memoria[param->sysCount + 1];
    incSysCount(param);
    *opcodeReturn = opcode;
    return true;
}

void decode(uint16_t opcode, uint8_t nibble[4]){
    uint8_t high_byte = (opcode >> 8);
    uint8_t low_byte = opcode & 0xFF;
    
    nibble[0] = (high_byte >> 4) & 0x0F;
    nibble[1] =  high_byte  & 0x0F;
    nibble[2] = (low_byte >> 4)  & 0x0F;
    nibble[3] = low_byte & 0x0F;
}

ExecuteStatus execute(Emulador * param, uint8_t nibble[4]){
    uint8_t NN = (nibble[2] << 4) | nibble[3];
    uint16_t NNN = (nibble[1] << 8) | (nibble[2] << 4) | nibble[3];

    switch (nibble[0]){
        case 0x0:{
            if(nibble[1] == 0x0 && nibble[2] == 0xE && nibble[3] == 0){
                memset(param->tela, false, sizeof(param->tela));
                return OK;
                // strcpy(retorno, "CLS");
            } else if(nibble[1] == 0x0 && nibble[2] == 0xE && nibble[3] == 0xE){
                bool sucess = false;
                uint16_t res = pilhaPop(param, &sucess);
                if(sucess == false){
                    return STACK_UNDERFLOW;
                } else{
                    param->sysCount = res;
                    return OK;
                }
                // strcpy(retorno, "RET");
            } else{
                return NOT_IMPLEMENTED;
                // snprintf(retorno, 50, "SYS 0x%03X", NNN);
            }
            break;
        }
        case 0x1:{
            param->sysCount = NNN;
            return OK;
            // snprintf(retorno, 50, "JP 0x%03X", NNN);
            break;
        }
        case 0x2:{
            if(pilhaPush(param, param->sysCount)){
                param->sysCount = NNN;
                return OK;
            } else{
                return STACK_OVERFLOW;
            }
            // snprintf(retorno, 50, "CALL 0x%03X", NNN);
            break;
        }
        case 0x3:{
            if(param->regV[nibble[1]] == NN){
                incSysCount(param);
            }
            return OK;
            // snprintf(retorno, 50, "SE V%X, 0x%02X", nibble[1], NN);
            break;
        }
        case 0x4:{
            if(param->regV[nibble[1]] != NN){
                incSysCount(param);
            }
            return OK;
            // snprintf(retorno, 50, "SNE V%X, 0x%02X", nibble[1], NN);
            break;
        }
        case 0x5:{
            if(nibble[3] == 0x0){
                if(param->regV[nibble[1]] == param->regV[nibble[2]]){
                    incSysCount(param);
                }
                return OK;
                // snprintf(retorno, 50, "SE V%X, V%X",nibble[1], nibble[2]);
            } else{
                return NOT_IMPLEMENTED;
                // strcpy(retorno, "????");
            }
            break;
        }
        case 0x6:{
            param->regV[nibble[1]] = NN;
            return OK;
            // snprintf(retorno, 50, "LD V%X, 0x%02X", nibble[1], NN);
            break;
        }
        case 0x7:{
            param->regV[nibble[1]] += NN;
            return OK;
            // snprintf(retorno, 50, "ADD V%X, 0x%02X", nibble[1], NN);
            break;
        }
        case 0x8:{
            if(nibble[3] == 0x0){
                param->regV[nibble[1]] = param->regV[nibble[2]];
                return OK;
                // snprintf(retorno, 50, "LD V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x1){
                param->regV[nibble[1]] = param->regV[nibble[1]] | param->regV[nibble[2]];
                return OK;
                // snprintf(retorno, 50, "OR V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x2){
                param->regV[nibble[1]] = param->regV[nibble[1]] & param->regV[nibble[2]];
                return OK;
                // snprintf(retorno, 50, "AND V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x3){
                param->regV[nibble[1]] = param->regV[nibble[1]] ^ param->regV[nibble[2]];
                return OK;
                // snprintf(retorno, 50, "XOR V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x4){
                uint16_t res = param->regV[nibble[1]] + param->regV[nibble[2]];
                if(res > UINT8_MAX){
                    param->regV[0xF] = 1;
                } else{
                    param->regV[0xF] = 0;
                }

                if(param->regV[nibble[1]] == param->regV[0xF]){
                    param->regV[0xF] = 0;
                }
                param->regV[nibble[1]];
                return OK;
                // snprintf(retorno, 50, "ADD V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x5){
                uint8_t res = param->regV[nibble[1]] - param->regV[nibble[2]];
                if(res < 0){
                    param->regV[0xF] = 0;
                } else{
                    param->regV[0xF] = 1;
                }

                if(param->regV[nibble[1]] == param->regV[0xF]){
                    param->regV[0xF] = 1;
                }
                return OK;
                // snprintf(retorno, 50, "SUB V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x6){
                return NOT_IMPLEMENTED;
                // snprintf(retorno, 50, "SHR V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x7){
                return NOT_IMPLEMENTED;
                // snprintf(retorno, 50, "SUBN V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0xE){
                return NOT_IMPLEMENTED;
                // snprintf(retorno, 50, "SHL V%X, V%X", nibble[1], nibble[2]);
            }else{
                return NOT_IMPLEMENTED;
                // strcpy(retorno, "????");
            }
            break;
        }
        case 0x9:{
            if(nibble[3] == 0x0){
                if(param->regV[nibble[1]] != param->regV[nibble[2]]){
                    incSysCount(param);
                }
                return OK;
                // snprintf(retorno, 50, "SNE V%X, V%X", nibble[1], nibble[2]);
            } else{
                return NOT_IMPLEMENTED;
                // strcpy(retorno, "????");
            }
            break;
        }
        case 0xA:{
            param->regI = NNN;
            return OK;
            // snprintf(retorno, 50, "LD I, 0x%03X", NNN);
            break;
        }
        case 0xB:{
            param->sysCount = NNN + param->regV[0];
            return OK;
            // snprintf(retorno, 50, "JP V0, 0x%03X",NNN);
            break;
        }
        case 0xC:{
            srand((unsigned int)time(NULL));
            param->regV[nibble[1]] = (rand() % 256) & NN;
            return OK;
            // snprintf(retorno, 50, "RND V%X, 0x%02X", nibble[1], NN);
            break;
        }
        case 0xD:{
            uint8_t sX = param->regV[nibble[1]];
            uint8_t sY = param->regV[nibble[2]];
            param->regV[0xF] = 0;
            for(int row = 0; row < nibble[3]; row++){
                if(sY + row >= HEIGHT) break;

                if(param->regI + row > MEMORY_S - 1) return STACK_OVERFLOW;
                uint8_t spriteB = param->memoria[param->regI + row];

                for(int col = 0; col < 8;col++){
                    if(sX + col >= WIDTH) break;

                    if((spriteB & (0x80 >> col)) != 0){
                        int posX = (sX + col);
                        int posY = (sY + row);
                        if(param->tela[posY][posX] == 1){
                            param->regV[0xF] = 1;
                        }

                        param->tela[posY][posX] ^= 1;
                    }
                }
            }
            return OK;
            // snprintf(retorno, 50, "DRW V%X, V%X, 0x%X", nibble[1],nibble[2], nibble[3]);
            break;
        }
        case 0xE:{
            if(nibble[2] == 0x9 && nibble[3] == 0xE){
                return NOT_IMPLEMENTED;
                // snprintf(retorno, 50, "SKP V%X",nibble[1]);
            } else if(nibble[2] == 0xA && nibble[3] == 0x1){
                return NOT_IMPLEMENTED;
                // snprintf(retorno, 50, "SKNP V%X",nibble[1]);
            } else{
                return NOT_IMPLEMENTED;
                // strcpy(retorno, "????");
            }
            break;
        }
        case 0xF:{
            if(nibble[2] == 0x0){
                if(nibble[3] == 0x7){
                    return NOT_IMPLEMENTED;
                    // snprintf(retorno, 50, "LD V%X, DT",nibble[1]);
                } else if(nibble[3] == 0xA){
                    return NOT_IMPLEMENTED;
                    // snprintf(retorno, 50, "LD V%X, K",nibble[1]);
                } else{
                    return NOT_IMPLEMENTED;
                    // strcpy(retorno, "????");
                }
            } else if (nibble[2] == 0x1){
                if(nibble[3] == 0x5){
                    return NOT_IMPLEMENTED;
                    // snprintf(retorno, 50, "LD DT, V%X",nibble[1]);
                } else if(nibble[3] == 0x8){
                    return NOT_IMPLEMENTED;
                    // snprintf(retorno, 50, "LD ST, V%X",nibble[1]);
                } else if(nibble[3] == 0xE){
                    param->regI += param->regV[nibble[1]];
                    return OK;
                    // snprintf(retorno, 50, "ADD I, V%X",nibble[1]);
                } else{
                    return NOT_IMPLEMENTED;
                    // strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x2){
                if(nibble[3] == 0x9){
                    return NOT_IMPLEMENTED;
                    // snprintf(retorno, 50, "LD F, V%X",nibble[1]);
                } else{
                    return NOT_IMPLEMENTED;
                    // strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x3){
                if(nibble[3] == 0x3){
                    return NOT_IMPLEMENTED;
                    // snprintf(retorno, 50, "LD B, V%X",nibble[1]);
                } else{
                    return NOT_IMPLEMENTED;
                    // strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x5){
                if(nibble[3] == 0x5){
                    for(int i = 0; i <= nibble[1]; i++){
                        if(param->regI + i < MEMORY_S - 1){
                            param->memoria[param->regI + i] = param->regV[i];
                        }else{
                            return STACK_OVERFLOW;
                        }
                    }
                    param->regI += nibble[1] + 1;
                    return OK;
                    // snprintf(retorno, 50, "LD [I], V%X",nibble[1]);
                } else{
                    return NOT_IMPLEMENTED;
                    // strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x6){
                if(nibble[3] == 0x5){
                    for(int i = 0; i <= nibble[1]; i++){
                        if(param->regI + i <= MEMORY_S - 1){
                            param->regV[i] = param->memoria[param->regI + i] ;
                        } else{
                            return STACK_OVERFLOW;
                        }
                    }
                    param->regI += nibble[1] + 1;
                    return OK;
                    // snprintf(retorno, 50, "LD V%X, [I]",nibble[1]);
                } else{
                    return NOT_IMPLEMENTED;
                    // strcpy(retorno, "????");
                }
            } else{
                return NOT_IMPLEMENTED;
                // strcpy(retorno, "????");
            }
            
            break;
        }
        default:{
            return NOT_IMPLEMENTED;
            // strcpy(retorno, "????");
            break;
        }
        return NOT_IMPLEMENTED;
    }
}



void disassemble(uint8_t nibble[4], char * retorno){
    uint8_t NN = (nibble[2] << 4) | nibble[3];
    uint16_t NNN = (nibble[1] << 8) | (nibble[2] << 4) | nibble[3];

    switch (nibble[0]){
        case 0x0:
            if(nibble[1] == 0x0 && nibble[2] == 0xE && nibble[3] == 0){
                strcpy(retorno, "CLS");
            } else if(nibble[1] == 0x0 && nibble[2] == 0xE && nibble[3] == 0xE){
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
            snprintf(retorno, 50, "SE V%X, 0x%02X", nibble[1], NN);
            break;
        case 0x4:
            snprintf(retorno, 50, "SNE V%X, 0x%02X", nibble[1], NN);
            break;
        case 0x5:
            if(nibble[3] == 0x0){
                snprintf(retorno, 50, "SE V%X, V%X",nibble[1], nibble[2]);
            } else{
                strcpy(retorno, "????");
            }
            break;
        case 0x6:
            snprintf(retorno, 50, "LD V%X, 0x%02X", nibble[1], NN);
            break;
        case 0x7:
            snprintf(retorno, 50, "ADD V%X, 0x%02X", nibble[1], NN);
            break;
        case 0x8:
            if(nibble[3] == 0x0){
                snprintf(retorno, 50, "LD V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x1){
                snprintf(retorno, 50, "OR V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x2){
                snprintf(retorno, 50, "AND V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x3){
                snprintf(retorno, 50, "XOR V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x4){
                snprintf(retorno, 50, "ADD V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x5){
                snprintf(retorno, 50, "SUB V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x6){
                snprintf(retorno, 50, "SHR V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0x7){
                snprintf(retorno, 50, "SUBN V%X, V%X", nibble[1], nibble[2]);
            } else if(nibble[3] == 0xE){
                snprintf(retorno, 50, "SHL V%X, V%X", nibble[1], nibble[2]);
            }else{
                strcpy(retorno, "????");
            }
            break;
        case 0x9:
            if(nibble[3] == 0x0){
                snprintf(retorno, 50, "SNE V%X, V%X", nibble[1], nibble[2]);
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
            snprintf(retorno, 50, "RND V%X, 0x%02X", nibble[1], NN);
            break;
        case 0xD:
            snprintf(retorno, 50, "DRW V%X, V%X, 0x%X", nibble[1],nibble[2], nibble[3]);
            break;
        case 0xE:
            if(nibble[2] == 0x9 && nibble[3] == 0xE){
                snprintf(retorno, 50, "SKP V%X",nibble[1]);
            } else if(nibble[2] == 0xA && nibble[3] == 0x1){
                snprintf(retorno, 50, "SKNP V%X",nibble[1]);
            } else{
                strcpy(retorno, "????");
            }
            break;
        case 0xF:
            if(nibble[2] == 0x0){
                if(nibble[3] == 0x7){
                    snprintf(retorno, 50, "LD V%X, DT",nibble[1]);
                } else if(nibble[3] == 0xA){
                    snprintf(retorno, 50, "LD V%X, K",nibble[1]);
                } else{
                    strcpy(retorno, "????");
                }
            } else if (nibble[2] == 0x1){
                if(nibble[3] == 0x5){
                    snprintf(retorno, 50, "LD DT, V%X",nibble[1]);
                } else if(nibble[3] == 0x8){
                    snprintf(retorno, 50, "LD ST, V%X",nibble[1]);
                } else if(nibble[3] == 0xE){
                    snprintf(retorno, 50, "ADD I, V%X",nibble[1]);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x2){
                if(nibble[3] == 0x9){
                    snprintf(retorno, 50, "LD F, V%X",nibble[1]);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x3){
                if(nibble[3] == 0x3){
                    snprintf(retorno, 50, "LD B, V%X",nibble[1]);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x5){
                if(nibble[3] == 0x5){
                    snprintf(retorno, 50, "LD [I], V%X",nibble[1]);
                } else{
                    strcpy(retorno, "????");
                }
            } else if(nibble[2] == 0x6){
                if(nibble[3] == 0x5){
                    snprintf(retorno, 50, "LD V%X, [I]",nibble[1]);
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


void printStatus(Emulador * param){
    printf("----------STATUS-----------\n");
    printf("I : %d\n", param->regI);
    for(int i = 0; i < 16; i++){
        printf("V%u : %d\n", i, param->regV[i]);
    }
    printf("sysCount : %u\n", param->sysCount);
}



void printScreen(Emulador * param){
    for(int i = 0; i <HEIGHT; i++){
        printf("\n");
        for(int j = 0; j<WIDTH; j++){
            if(param->tela[i][j] == true){
                printf("#");
            } else{
                printf(" ");
            }
        }
    }
}   