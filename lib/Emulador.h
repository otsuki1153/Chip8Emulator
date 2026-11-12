#ifndef EMULATOR
#define EMULATOR

#include <stdint.h>
#include <stdbool.h>
#define MEMORY_S 4096
#define STACK_S 16
#define START_ADDR 0x200
#define WIDTH 64
#define HEIGHT 32

typedef struct Emulador{
    bool tela[HEIGHT][WIDTH];
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
bool fetch(Emulador * param, uint16_t *opcodeReturn);
void decode(uint16_t opcode, uint8_t nibble[4]);

typedef enum ExecuteStatus{
    OK,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    NOT_IMPLEMENTED
} ExecuteStatus;

ExecuteStatus execute(Emulador * param, uint8_t nibble[4]);

void cleanTerminal();

/**
 * @brief Com as partes já separadas do opcode verifica cada caractere em cadeia para traduzir o comando em hexadecimal de dois bytes em um buffer de texto de comando Assembly que é atribuido a variável em que foi passada o endereço dela por parâmetro  
 * 
 * @param[in] param (Emulador*) endereço da struct emulador
 * @param[out] retorno (char*) Endereço onde irá receber o buffer com o comando traduzido
 * @return nada, apenas atribui o valor do buffer a variável que foi passado o endereço por parâmetro
 */
void disassemble(uint8_t nibble[4], char * retorno);

/**
 * @brief pega os dados de registradores V, registrador I e o contador do sistema (sysCount) e printa essas informações no terminal
 * 
 * @param[in] param (Emulador*) endereço da struct emulador
 * @return nada, só printa os status dos registradores e o contador no terminal
 */
void printStatus(Emulador * param);

/**
 * @brief Pega o buffer da tela da struct emulador e printa * para caso seja verdadeiro e " " para caso seja falso
 * 
 * @param[in] param (Emulador*) endereço da struct emulador
 * @return nada, só printa a tela de acordo com o buffer do emulador
 */
void printScreen(Emulador * param);
#endif // !EMULATOR