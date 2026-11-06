# CHIP-8 em C: disassembler e emulador

Projeto de estudo para aprender, na prática, como um emulador funciona por dentro. O alvo é o **CHIP-8**, uma máquina virtual simples (~35 instruções, 4 KB de memória, tela monocromática de 64×32), que serve de porta de entrada para emular hardware real, como Game Boy e NES.

Tudo é escrito em **C**, sem frameworks, com a ideia de entender cada camada em vez de usar algo pronto.

## Objetivos

- Entender o ciclo **fetch → decode → execute** de uma CPU.
- Praticar manipulação de bits (máscaras, deslocamentos, big-endian vs. little-endian).
- Aprender a organizar um projeto em C com vários arquivos (`.h` / `.c`) e compilar pelo terminal.
- Validar cada etapa com ROMs de teste, em vez de apenas "jogar e ver se funciona".

## Status

| Etapa | Situação |
|---|---|
| Disassembler de CHIP-8 | Concluído |
| Emulador, etapa 1: memória, registradores, pilha e carga da ROM em `0x200` | Concluída |
| Emulador, etapa 2: ciclo fetch → decode → execute | Em andamento |
| Tela (`DXYN`), saltos, sub-rotinas, aritmética, timers e teclado | Planejado |
| Janela com SDL2 | Planejado |

## Estrutura

```
.
├── IBM Logo.ch8        # ROMs usadas nos testes
├── lib/
│   ├── Emulador.h      # struct Emulador, constantes e declarações
│   └── Emulador.c      # inicialização, pilha e tradução de opcodes
└── src/
    └── main.c          # programa de teste / ponto de entrada
```

### O que há em `Emulador.h`

- `memoria[4096]`: memória do CHIP-8. A ROM é carregada a partir do endereço `0x200`.
- `regV[16]`: registradores `V0` a `VF`.
- `regI`: registrador de índice.
- `sysCount`: contador de programa (endereço da próxima instrução).
- `pilha[16]` e `posPilha`: pilha de endereços de retorno, com `pilhaPush` e `pilhaPop` que detectam overflow e underflow.
- `decode(...)`: traduz um opcode de 2 bytes para texto (`6XNN` → `LD V1, 0x08`), útil para depurar a execução.

## Como compilar e rodar

Requisitos: `gcc` e um terminal. Os comandos assumem que você está **na pasta do `main.c`**, porque o caminho da ROM é relativo.

```bash
gcc -Wall -Wextra main.c ../lib/Emulador.c -o emulador
./emulador
```

Todos os arquivos `.c` precisam aparecer no comando. Os `.h` entram sozinhos pelos `#include`.

### Disassembler

O disassembler lê uma ROM e imprime cada par de bytes como instrução, com o endereço:

```
0x0200 : 00E0 -> CLS
0x0202 : A22A -> LD I, 0x22A
0x0204 : 600C -> LD V0, 0x0C
0x0206 : 6108 -> LD V1, 0x08
0x0208 : D01F -> DRW V0, V1, 0xF
```

Ao executar, um menu permite escolher a ROM a desmontar.

## Como o CHIP-8 foi decodificado

- Cada opcode tem **2 bytes** e é lido em **big-endian**: o primeiro byte do arquivo é a parte alta.
- O primeiro nibble escolhe a família da instrução; os outros formam `X`, `Y`, `N`, `NN` ou `NNN`, conforme a instrução.
- Os bytes são lidos um a um para um array, e o opcode é montado com deslocamento e OR. Assim o resultado não depende da arquitetura da máquina em que o programa roda.
- O contador de programa avança 2 logo após o *fetch*, de modo que instruções de salto só precisam sobrescrevê-lo.
- Os mnemônicos seguem a convenção da referência do Cowgod (`LD`, `SE`, `SNE`, `JP`, `DRW`, `RND`...).

## Roadmap de estudo

1. **Disassembler de CHIP-8**: concluído.
2. **Emulador de CHIP-8**: núcleo, depois tela e janela.
3. **Debugger** embutido (breakpoints, execução passo a passo, dump de estado).
4. **Intel 8080 + Space Invaders**: primeira CPU real.
5. **Game Boy**, em marcos testáveis (CPU, memória, timers, PPU, joypad).
6. **NES** (6502), e um projeto autoral a escolher.

## Limitações conhecidas

- O disassembler é **linear**: ele lê a ROM de ponta a ponta e não separa código de dados. Por isso, regiões de sprites aparecem como instruções estranhas (`????`, `SYS ...`). Não é um defeito da decodificação.
- ROMs com número ímpar de bytes têm o último byte tratado de forma especial.
- Ainda não há tela, teclado nem som.

## Testes

A validação usa a **suíte de testes do Timendus** e a ROM *IBM Logo*. A saída do `decode` do emulador é conferida contra a do disassembler, linha a linha.

## Referências

- [Guia "How to write an emulator (CHIP-8 interpreter)", de Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Tabela de opcodes do CHIP-8 (Gulrak)](https://chip8.gulrak.net)
- [Suíte de testes do Timendus](https://github.com/Timendus/chip8-test-suite)
- [Cowgod's CHIP-8 Technical Reference (convenção de mnemônicos)](https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod's%20CHIP-8%20Technical%20Reference.pdf)
- Comunidade EmuDev (Discord e r/EmuDev) *sem link

## Licença

A definir.
