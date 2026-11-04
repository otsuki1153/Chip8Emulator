#ifndef EMULATOR
#define EMULATOR

#include <stdint.h>
#include <stdbool.h>
#define MEMORY_S 4096
#define STACK_S 16
#define START_ADDR 0x200

typedef struct Emulador{
    uint8_t memoria[MEMORY_S];
    uint16_t pilha [STACK_S];
    uint8_t posPilha;
    uint8_t regV[16];
    uint16_t regI;
    uint16_t sysCount;
} Emulador;

void initEmulator(Emulador * param);  
void incSysCount(Emulador * param);
bool pilhaPush(Emulador * param, uint16_t address);
uint16_t pilhaPop(Emulador * param, bool * success);
void decode(uint8_t high, uint8_t low, char *retorno);
#endif // !EMULATOR