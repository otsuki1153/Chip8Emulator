#include <stdio.h>
#include "../lib/Emulador.h"

int main(){
    FILE * arquivo = fopen("../IBM Logo.ch8", "rb");
    Emulador emu;
    initEmulator(&emu);

    if(arquivo == NULL){
        printf("Erro ao abrir\n");
        return 1; 
    }

    fseek(arquivo, 0, SEEK_END);

    long tamanho = ftell(arquivo);

    if(tamanho == -1L){
        printf("Erro ao ler o tamanho\n");
        fclose(arquivo);
        return 1;
    }

    if(tamanho > 3584){
        printf("Arquivo muito Grande\n");
        fclose(arquivo);
        return 1;
    }

    fseek(arquivo, 0, SEEK_SET);
    fread(&emu.memoria[START_ADDR], 1 , tamanho, arquivo);

    for(int i = START_ADDR; i < tamanho; i++){
        printf("%02X", emu.memoria[i]);
    }

    fclose(arquivo);
    return 0;
}