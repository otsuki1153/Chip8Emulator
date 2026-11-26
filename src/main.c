#include <stdio.h>
#include "../lib/Emulador.h"
#include "../lib/display.h"

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
    SDL_Window *window = NULL;
    SDL_Renderer * renderer = NULL;
    SDL_Texture * textura = NULL;
    SDL_WindowFlags flag = 0;
    
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

    if(!initDisplay(&window, &renderer, flag, &textura)){
        printf("ERRO AO INICIAR O DISPLAY");
        return 1;
    }

    int conf = 0;

    printf("Escolha a configuração que quer rodar o emulador(padrão 1):\n");
    printf("1. Antiga\n");
    printf("2. Recente\n");
    scanf("%d", &conf);
    getchar();

    if(conf > 2 || conf <= 0){
        printf("Opção inválida, selecionado opção 1");
        conf = 1;
    }

    emu.legacy_mode = conf == 1 ? true : false; 
    
    char  comando[50]; 
    uint16_t opcode = 0x0;
    uint8_t nibble[4];
    uint16_t sysCountStamp = 0x0;
    ExecuteStatus execStatus = OK;
    TimerStatus timerStatus = CHILL;
    const double freq = (double)SDL_GetPerformanceFrequency();
    const double Target_FPS = 1.0/60.0;  
 
    fseek(arquivo, 0, SEEK_SET);
    if(fread(&emu.memoria[START_ADDR], 1 , tamanho, arquivo) == tamanho){
        bool error = false;
        emu.memoria[0x1FF] = 1;
        int n = emu.legacy_mode ? 50 : 10;
        while(!error){
            // printStatus(&emu);
            uint64_t startfreq = SDL_GetPerformanceCounter();
            for(int i = 0; i < n; i++){
                execStatus = OK;
                runEmulator(&emu, comando, &opcode, nibble, &sysCountStamp, &execStatus, &error);
                if(emu.legacy_mode){
                    if(error || execStatus == DRAW_EXECUTED){
                        break;
                    }
                } else{
                    if(error){
                        break;
                    }
                }
            }
            printDisplay(&emu, textura, renderer);
            uint64_t finalFreq = SDL_GetPerformanceCounter();
            double time = (double)(finalFreq - startfreq)/freq;
            timerStatus = decTimers(&emu);
            if(timerStatus == DELAY_TIMER_UNDERFLOW){
                printf("\nErro de DELAY_TIMER_UNDERFLOW: houve uma tentativa de decrementar o valor do timer do delay para o valor menor que zero\n");
            } else if(timerStatus == SOUND_TIMER_UNDERFLOW){
                printf("\nErro de SOUND_TIMER_UNDERFLOW: houve uma tentativa de decrementar o valor do timer do som para o valor menor que zero\n");
            }
            if(time < Target_FPS){
                double time_left = Target_FPS - time;
                SDL_DelayNS((Uint64)(time_left * 1e9));
            }
        }
        
    } else{
        printf("erro na leitura do arquivo");
    }

    fclose(arquivo);
    return 0;
}
