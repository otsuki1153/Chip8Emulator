#ifndef EMULATOR
#define EMULATOR

#include <stdint.h>
#define MEMORY_L 4096

typedef struct Emulador {
    uint8_t memoria[MEMORY_L];
    uint8_t regV[16];
    uint16_t regI;
    uint16_t contador;
} Emulador;

void createEmulator(Emulador * param);  
void incContador(Emulador * param);
#endif // !EMULATOR