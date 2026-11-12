#include <stdio.h>
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
    uint16_t sysCountStamp = 0x0;
    ExecuteStatus execStatus = OK;
 
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
            void cleanTerminal();
            sysCountStamp = emu.sysCount;
            if(!fetch(&emu, &opcode))printf("Program Counter excedeu a memória");
            decode(opcode, nibble);
            disassemble(nibble, comando);
            execStatus = execute(&emu, nibble);

            if(execStatus == OK){
                printScreen(&emu);
            } else if(execStatus == NOT_IMPLEMENTED){
                printf("_____OPCODE NÃO IDENTIFICADO_____");
                printf("pc: %04X, opcode: %04X, comando: %s", sysCountStamp,opcode, comando);
                printf("_________________________________");
                break;
            } else if (execStatus == STACK_OVERFLOW){
                printf("Erro de Stack Overflow: houve uma tentativa de acesso a uma posição inexistente acima do tamanho da stack");
            } else{
                printf("Erro de Stack Underflow: houve uma tentativa de acesso a uma posição inexistente abaixo do tamanho da stack");
            }
        }
        
    } else{
        printf("erro na leitura do arquivo");
    }

    fclose(arquivo);
    return 0;
}
