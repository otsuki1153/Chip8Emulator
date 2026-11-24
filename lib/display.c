#include<SDL3/SDL.h>
#include<SDL3/SDL_main.h>
#include "display.h"

bool initDisplay(SDL_Window * window, SDL_Renderer * renderer, SDL_WindowFlags flags, SDL_Texture * texture){
    if(!SDL_Init(SDL_INIT_VIDEO)){
        SDL_Log("Erro ao inicializar SDL3: %s", SDL_GetError());
        return false;
    }

    window = NULL;
    renderer = NULL;
    SDL_WindowFlags window_flags = flags;

    if(!SDL_CreateWindowAndRenderer("Janela SDL3", 640,320,window_flags, &window, &renderer)){
        SDL_Log("Erro ao criar janela/renderizador: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);

    if(!texture){
        SDL_Log("Erro ao criar a Textura: %s", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

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