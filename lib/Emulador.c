#include "Emulador.h"
#include <string.h>


void createEmulator(Emulador * param){
    param->contador = 0x200;
    param->regI = 0;
    memset(param->memoria, 0,sizeof(param->memoria));
}

void incContador(Emulador * param){
    param->contador += 2;
}

