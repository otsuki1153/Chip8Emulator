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

    long unsigned int tamanho = ftell(arquivo);

    if(tamanho == -1UL){
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
    if(fread(&emu.memoria[START_ADDR], 1 , tamanho, arquivo) == tamanho){
        for(long unsigned int i = START_ADDR; i < START_ADDR + tamanho; i = i+2){
            char  comando[50]; 
            // decode(emu.memoria[i], emu.memoria[i+1], comando);
            // printf("%02X %02X ", emu.memoria[i], emu.memoria[i + 1]); 
            printf("%s\n", comando);
        }

    } else{
        printf("erro na leitura do arquivo");
    }


    

    fclose(arquivo);
    return 0;
}