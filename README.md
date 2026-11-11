# CHIP-8 em C: [disassembler](https://github.com/otsuki1153/Disassembler-Model) e emulador

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

O repositório também inclui um executável Linux já compilado em `src/emulador`. A partir da pasta `src`, rode `./emulador`. Se o sistema bloquear a execução por falta de permissão, habilite-a com `chmod +x emulador` e tente novamente. Para recompilar, use os comandos acima.

O programa abre `../IBM Logo.ch8`, carrega a ROM em `0x200`, imprime o estado dos registradores durante a execução e desenha o conteúdo do buffer gráfico no terminal. O laço final de espera mantém o processo aberto depois da execução.

## Componentes principais

- `Emulador` reúne memória, tela, registradores, pilha e contador de programa.
- `initEmulator` zera o estado e inicia `sysCount` em `0x200`.
- `fetch` lê o opcode na posição atual e avança `sysCount` em dois bytes.
- `decode` separa o opcode em quatro nibbles; `execute` aplica as operações implementadas.
- `disassemble` produz o mnemônico textual do opcode, independentemente de sua execução estar implementada.
- `printStatus` e `printScreen` exibem o estado da máquina no terminal.

## Como o emulador funciona

O CHIP-8 executa instruções de 16 bits (dois bytes). O programa começa no endereço `0x200`; cada ciclo busca a instrução na memória, separa seus campos e aplica a operação correspondente:

1. **Carregar a ROM:** `main.c` abre `IBM Logo.ch8` e copia seus bytes para a memória a partir de `0x200`.
2. **Buscar (fetch):** `fetch` combina dois bytes em um opcode big-endian e avança `sysCount` em dois.
3. **Decodificar (decode):** `decode` divide o opcode em quatro nibbles. Por exemplo, `6A12` vira `6`, `A`, `1`, `2`.
4. **Executar (execute):** `execute` interpreta os nibbles. Saltos e instruções de skip alteram `sysCount`; as demais operações implementadas atualizam registradores, índice ou tela.
5. **Exibir:** `main.c` imprime o estado antes de cada ciclo e, ao sair do laço, mostra o buffer gráfico no terminal.

O contador já aponta para a próxima instrução quando `execute` é chamado. Por isso, um salto define diretamente o novo endereço, e uma instrução de skip avança mais dois bytes para pular a próxima instrução.

### Estado da máquina

| Campo | Função |
|---|---|
| `memoria[4096]` | Memória de 4 KB; a ROM é carregada a partir de `0x200`. |
| `regV[16]` | Registradores de 8 bits `V0` a `VF`; `VF` também é usado como flag de colisão no desenho. |
| `regI` | Registrador de índice, usado para endereçar dados e sprites. |
| `sysCount` | Contador de programa: endereço da próxima instrução a buscar. |
| `pilha[16]`, `posPilha` | Espaço para endereços de retorno de sub-rotinas; push/pop estão implementados, mas ainda não são usados por `CALL`/`RET`. |
| `tela[32][64]` | Buffer monocromático; `true` representa pixel aceso. |

### Instruções com execução implementada

`X` e `Y` representam registradores, `NN` um byte imediato, `NNN` um endereço de 12 bits e `N` um nibble.

| Opcode | Efeito implementado |
|---|---|
| `00E0` | Limpa o buffer da tela. |
| `1NNN` | Salta para o endereço `NNN`. |
| `3XNN`, `4XNN` | Pula a próxima instrução se `VX` for igual/diferente de `NN`. |
| `5XY0`, `9XY0` | Pula a próxima instrução se `VX` for igual/diferente de `VY`. |
| `6XNN`, `7XNN` | Carrega `NN` em `VX` ou soma `NN` a `VX` (com o limite natural de 8 bits). |
| `8XY0`–`8XY3` | Copia `VY` para `VX`, ou aplica OR, AND ou XOR entre eles. |
| `ANNN` | Carrega `NNN` em `I`. |
| `BNNN` | Salta para `NNN + V0`. |
| `DXYN` | Desenha até `N` linhas de sprite apontadas por `I` nas coordenadas `VX`, `VY`; pixels são alternados (XOR) e `VF` recebe 1 se algum pixel aceso foi apagado. |
| `FX1E` | Soma `VX` a `I`. |

O disassembler também reconhece outros opcodes, mas isso não significa que o executor os implemente. OpCodes não listados aqui ainda não têm efeito funcional em `execute`.

### O que falta fazer

| Prioridade sugerida | Trabalho | Resultado esperado |
|---|---|---|
| 1 | Ligar `2NNN` e `00EE` às funções de pilha | Chamada e retorno corretos de sub-rotinas. |
| 2 | Completar instruções `8XY*`, `CXNN`, `EX9E` e `EXA1` | Operações aritméticas, aleatórias e skips por teclado. |
| 3 | Implementar timers e instruções `FX**` restantes | Temporizador de atraso/som e operações de memória/fontes. |
| 4 | Adicionar entrada de teclado e temporização de 60 Hz | Execução interativa e comportamento temporal CHIP-8. |
| 5 | Validar limites e erros do carregamento/acesso à memória | Evitar leitura fora da memória e informar ROM inválida. |
| 6 | Adicionar testes por instrução e ROMs de validação | Conferir efeitos em registradores, memória e tela. |
| 7 | Criar uma janela gráfica | Apresentar pixels e receber entrada de forma visual. |

## Limitações conhecidas

- O carregador no `main.c` limita a ROM a 3.584 bytes, para caber na memória a partir de `0x200`.
- Não há validação de limites de memória no `fetch` nem no acesso aos dados dos sprites.
- `CALL` (`2NNN`) e `RET` (`00EE`) são reconhecidos pelo disassembler, mas ainda não são executados; as funções da pilha existem, mas não estão ligadas a essas instruções.
- A maior parte das instruções aritméticas, de teclado, timers e som ainda não é executada.
- Não há janela gráfica nem entrada de teclado; a tela é impressa como texto.
- O desenho interrompe o sprite nas bordas, em vez de fazê-lo voltar ao lado oposto.
- O laço principal de `main.c` usa a repetição do valor do contador de programa como condição de parada; não é um mecanismo geral de término de ROM.

## Disassembler

O [disassembler](https://github.com/otsuki1153/Disassembler-Model) é um projeto separado que lê uma ROM e imprime os opcodes como mnemônicos. A análise é linear: interpreta cada par de bytes como uma instrução, sem distinguir código de dados. Bytes de sprites ou dados podem, portanto, aparecer como instruções inválidas ou inesperadas.

## Próximos passos possíveis

- Implementar `CALL` e `RET` usando a pilha existente.
- Completar a execução dos grupos aritmético, teclado e timers.
- Adicionar carregamento de ROM com validações de tamanho e de acesso à memória.
- Implementar temporização e uma interface gráfica com teclado.
- Criar testes automatizados para as instruções já implementadas.

## Referências

- [Como escrever um interpretador CHIP-8 — Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Tabela de opcodes CHIP-8 — Gulrak](https://chip8.gulrak.net)
- [Suíte de testes CHIP-8 — Timendus](https://github.com/Timendus/chip8-test-suite)
- [Referência técnica CHIP-8 de Cowgod](https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod's%20CHIP-8%20Technical%20Reference.pdf)

## Licença

A definir.
