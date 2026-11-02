#include "Emulador.h"
#include <string.h>


void initEmulator(Emulador * param){
    param->sysCount = 0x200;
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

