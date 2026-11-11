#include <stdio.h>
#include <stdlib.h>
#include "../lib/Emulador.h"

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Muitos argumentos");
        printf("Numero de argumentos: %d", argc);
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
    // uint16_t sysCountStamp = 0x0;

 
    fseek(arquivo, 0, SEEK_SET);
    if(fread(&emu.memoria[START_ADDR], 1 , tamanho, arquivo) == tamanho){
        // while(emu.sysCount != sysCountStamp){
        //     printStatus(&emu);
        //     sysCountStamp = emu.sysCount;
        //     fetch(&emu, &opcode);
        //     decode(opcode, nibble);
        //     disassemble(nibble, comando);
        //     execute(&emu, nibble);
        //     // printf("%s\n", comando);
        // }
        while(true){
            // printStatus(&emu);
            #ifdef _WIN32
                system("cls");
            #else
                system("clear");
            #endif
            fetch(&emu, &opcode);
            decode(opcode, nibble);
            disassemble(nibble, comando);
            execute(&emu, nibble);
            printScreen(&emu);
            // printf("%s\n", comando);
        }
        
    } else{
        printf("erro na leitura do arquivo");
    }

    fclose(arquivo);
    return 0;
}
