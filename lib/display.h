#ifndef DISPLAY
#define DISPLAY 

#define HIGH_COLOR 0xFFFFFFF
#define LOW_COLOR 0x1E1E28FF

#include "Emulador.h"
#include<SDL3/SDL.h>

bool initDisplay(SDL_Window ** window, SDL_Renderer ** renderer, SDL_WindowFlags flags, SDL_Texture ** texture);
void printDisplay(Emulador * param,  SDL_Texture * texture, SDL_Renderer * renderer);
void keyPadProcess(Emulador * param);

#endif // !DISPLAY