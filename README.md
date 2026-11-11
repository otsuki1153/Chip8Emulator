# Emulador CHIP-8 em C

Projeto de estudo para explorar o funcionamento de um interpretador CHIP-8 e de um disassembler. O núcleo está escrito em C e, no estado atual, executa algumas instruções e desenha em uma tela textual no terminal.

## Estado atual

O projeto inclui:

- Estado da máquina com 4.096 bytes de memória, 16 registradores `V0`–`VF`, registrador `I`, contador de programa, pilha de 16 posições e tela monocromática de 64 × 32.
- Inicialização da máquina e operações de push/pop da pilha, com retorno indicando overflow ou underflow.
- Busca de opcodes de dois bytes em big-endian, separação em nibbles e avanço do contador de programa.
- Disassembler que converte opcodes em mnemônicos para depuração.
- Execução implementada para `00E0`, `1NNN`, `3XNN`, `4XNN`, `5XY0`, `6XNN`, `7XNN`, `8XY0`–`8XY3`, `9XY0`, `ANNN`, `BNNN`, `DXYN` e `FX1E`.
- Desenho de sprites na tela textual e indicação de colisão em `VF`.
- Um programa de entrada que carrega a ROM `IBM Logo.ch8`, exibe o estado dos registradores e tenta executar o programa.

O disassembler reconhece mais instruções do que o executor implementa. Instruções sem implementação no `execute` não alteram o estado da máquina (com exceção do avanço do contador feito por `fetch`). A ROM IBM Logo pode não terminar: o programa não implementa um ciclo de temporização, teclado ou outras instruções necessárias para compatibilidade geral.

## Estrutura

```text
.
├── IBM Logo.ch8        # ROM de exemplo
├── lib/
│   ├── Emulador.h      # Estrutura, constantes e declarações da API
│   └── Emulador.c      # Estado, ciclo de instruções, disassembler e saída no terminal
└── src/
    └── main.c          # Carrega a ROM e executa o ciclo de teste
```

## Compilar e executar

Requisitos: GCC e terminal. Execute os comandos a partir da pasta `src`, pois o caminho para a ROM é relativo a essa pasta:

```sh
gcc -Wall -Wextra main.c ../lib/Emulador.c -o emulador
./emulador
```

No Windows, execute o binário gerado com `emulador.exe`.

O programa abre `../IBM Logo.ch8`, carrega a ROM em `0x200`, imprime o estado dos registradores durante a execução e desenha o conteúdo do buffer gráfico no terminal. O laço final de espera mantém o processo aberto depois da execução.

## Componentes principais

- `Emulador` reúne memória, tela, registradores, pilha e contador de programa.
- `initEmulator` zera o estado e inicia `sysCount` em `0x200`.
- `fetch` lê o opcode na posição atual e avança `sysCount` em dois bytes.
- `decode` separa o opcode em quatro nibbles; `execute` aplica as operações implementadas.
- `disassemble` produz o mnemônico textual do opcode, independentemente de sua execução estar implementada.
- `printStatus` e `printScreen` exibem o estado da máquina no terminal.

## Limitações conhecidas

- O carregador no `main.c` limita a ROM a 3.584 bytes, para caber na memória a partir de `0x200`.
- Não há validação de limites de memória no `fetch` nem no acesso aos dados dos sprites.
- `CALL` (`2NNN`) e `RET` (`00EE`) são reconhecidos pelo disassembler, mas ainda não são executados; as funções da pilha existem, mas não estão ligadas a essas instruções.
- A maior parte das instruções aritméticas, de teclado, timers e som ainda não é executada.
- Não há janela gráfica nem entrada de teclado; a tela é impressa como texto.
- O desenho interrompe o sprite nas bordas, em vez de fazê-lo voltar ao lado oposto.
- O laço principal de `main.c` usa a repetição do valor do contador de programa como condição de parada; não é um mecanismo geral de término de ROM.

## Disassembler

O disassembler é linear: interpreta cada par de bytes como uma instrução, sem distinguir código de dados. Bytes de sprites ou dados podem, portanto, aparecer como instruções inválidas ou inesperadas.

## Próximos passos possíveis

- Implementar `CALL` e `RET` usando a pilha existente.
- Completar a execução dos grupos aritmético, teclado e timers.
- Adicionar carregamento de ROM com validações de tamanho e de acesso à memória.
- Implementar temporização e uma interface gráfica com teclado.
- Criar testes automatizados para as instruções já implementadas.

## Referências

- [Meu projeto de disassembler CHIP-8](https://github.com/otsuki1153/Disassembler-Model)
- [Como escrever um interpretador CHIP-8 — Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Tabela de opcodes CHIP-8 — Gulrak](https://chip8.gulrak.net)
- [Suíte de testes CHIP-8 — Timendus](https://github.com/Timendus/chip8-test-suite)
- [Referência técnica CHIP-8 de Cowgod](https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod's%20CHIP-8%20Technical%20Reference.pdf)

## Licença

A definir.
