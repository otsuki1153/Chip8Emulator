#include <stdio.h>
#include "../lib/Emulador.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        if(argc > 2){
            printf("Muitos argumentos\n");
        } else{
            printf("Poucos argumentos\n");
        }
        printf("Numero de argumentos: %d\n", argc);
        return 1;
    } 

    FILE * arquivo = fopen(argv[1], "rb");
    
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

    char  comando[50]; 
    uint16_t opcode = 0x0;
    uint8_t nibble[4];
    uint16_t sysCountStamp = 0x0;
    ExecuteStatus execStatus = OK;
 
    fseek(arquivo, 0, SEEK_SET);
    if(fread(&emu.memoria[START_ADDR], 1 , tamanho, arquivo) == tamanho){
        bool error = false;
        while(!error){
            // printStatus(&emu);
            emu.memoria[0x1FF] = 1;
            for(int i = 0; i < N; i++){
                runEmulator(&emu, comando, &opcode, nibble, &sysCountStamp, &execStatus, &error);
                if(error){
                    break;
                }
            }
            decTimers(&emu);
        }
        
    } else{
        printf("erro na leitura do arquivo");
    }

    fclose(arquivo);
    return 0;
}
