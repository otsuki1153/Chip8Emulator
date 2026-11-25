#include "display.h"

bool initDisplay(SDL_Window ** window, SDL_Renderer ** renderer, SDL_WindowFlags flags, SDL_Texture ** texture){
    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erro ao inicializar SDL3: %s", SDL_GetError());
        return false;
    }


    if(!SDL_CreateWindowAndRenderer("Janela SDL3", 640,320,flags, window, renderer)){
        SDL_Log("Erro ao criar janela/renderizador: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if(*texture == NULL){
        SDL_Log("Erro ao criar a Textura: %s", SDL_GetError());
        SDL_DestroyRenderer(*renderer);
        SDL_DestroyWindow(*window);
        *renderer = NULL;
        *window = NULL;
        SDL_Quit();
        return false;
    }

    SDL_SetTextureScaleMode(*texture, SDL_SCALEMODE_NEAREST);
    return true;
}

void printDisplay(Emulador * param,  SDL_Texture * texture, SDL_Renderer * renderer){
    uint32_t pixelsRGBA[HEIGHT][WIDTH];
    for(int i = 0; i < HEIGHT;i++){
        for(int j = 0; j < WIDTH; j++){
            pixelsRGBA[i][j] = param->tela[i][j] ? HIGH_COLOR : LOW_COLOR;
        }
    }

    SDL_UpdateTexture(texture,NULL, pixelsRGBA, WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}