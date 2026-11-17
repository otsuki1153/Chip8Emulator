# CHIP-8 em C: [disassembler](https://github.com/otsuki1153/Disassembler-Model) e emulador

Projeto de estudo sobre emulação CHIP-8, escrito em C. O programa carrega uma ROM indicada na linha de comando, executa as instruções implementadas e desenha a tela no terminal. O projeto de disassembler ligado no título é separado deste emulador.

## Estado do projeto

O núcleo já implementa o ciclo de busca, decodificação e execução, uma parte das instruções CHIP-8, a pilha de sub-rotinas e o desenho de sprites. O emulador ainda não tem teclado, temporizadores, som ou janela gráfica; ROMs que dependem desses recursos podem parar ao encontrar uma instrução não implementada.

### Funcionalidades e próximos passos

| Área | Implementado | Falta fazer |
|---|---|---|
| Máquina | Memória de 4 KB, registradores `V0`–`VF`, `I`, contador de programa e tela 64 × 32. | Completar a validação de limites e tratar falhas de leitura do contador de programa. |
| Pilha | Pilha de 16 posições; `CALL`/`RET` retornam erro de overflow/underflow. | — |
| Instruções | Conjunto listado na tabela abaixo, incluindo aritmética, saltos, desenho e operações de memória. | Implementar os opcodes restantes e conferir comportamentos de compatibilidade CHIP-8. |
| ROM | Recebe o caminho da ROM como argumento; aceita arquivos de até 3.584 bytes. | Melhorar mensagens e tratamento de erros de arquivo. |
| Tela | Desenha os pixels no terminal e limpa a tela entre ciclos. | Adicionar janela gráfica e entrada de teclado. |
| Temporização e som | — | Implementar timers CHIP-8, relógio de execução e som. |
| Validação | Há ROMs de teste no repositório. | Executar e validar as ROMs após implementar teclado, timers e opcodes restantes. |

### Instruções implementadas

`X` e `Y` indicam registradores, `NN` é um valor de 8 bits, `NNN` um endereço de 12 bits e `N` um valor de 4 bits.

| Opcode(s) | Comportamento |
|---|---|
| `00E0`, `00EE` | Limpa a tela; retorna de uma sub-rotina. |
| `1NNN`, `2NNN` | Salta para `NNN`; chama uma sub-rotina em `NNN`. |
| `3XNN`, `4XNN`, `5XY0`, `9XY0` | Pula a instrução seguinte conforme a comparação de registradores e valores. |
| `6XNN`, `7XNN` | Carrega valor imediato em `VX`; soma valor imediato a `VX`. |
| `8XY0`–`8XY7`, `8XYE` | Cópia, operações OR/AND/XOR, soma, subtração e deslocamentos. |
| `ANNN`, `BNNN` | Define `I`; salta para `NNN + V0`. |
| `CXNN` | Gera um valor aleatório e aplica a máscara `NN`. |
| `DXYN` | Desenha sprite de `N` linhas usando os dados em `I`; registra colisão em `VF`. |
| `FX1E` | Soma `VX` a `I`. |
| `FX33`, `FX55`, `FX65` | Escreve os dígitos decimais de `VX` na memória; salva ou carrega registradores em memória. |

Os deslocamentos `8XY6` e `8XYE` copiam primeiro `VY` para `VX` e deslocam `VX`. As operações `FX55` e `FX65` também avançam `I` após transferir os registradores. O desenho para nas bordas da tela, sem voltar ao lado oposto.

## Como funciona

1. `main.c` recebe um caminho de ROM, abre o arquivo e copia seus bytes para a memória a partir do endereço `0x200`.
2. `fetch` lê dois bytes em big-endian para formar um opcode e avança o contador de programa em dois.
3. `decode` divide o opcode em quatro nibbles. Por exemplo, `6A12` é separado em `6`, `A`, `1` e `2`.
4. `execute` aplica a instrução: pode atualizar registradores, pilha, memória, tela ou contador de programa.
5. A tela é redesenhada no terminal a cada instrução. Se o executor retornar uma instrução não implementada ou um erro de pilha/memória, o programa mostra a mensagem e encerra o ciclo.

## Arquivos do projeto

```text
.
├── IBM_Logo.ch8             # ROM de exemplo
├── 1-chip8-logo.ch8          # Teste do logo CHIP-8 (Timendus)
├── 3-corax+.ch8              # Teste de opcodes (Timendus)
├── 4-flags.ch8               # Teste de flags (Timendus)
├── 5-quirks.ch8              # Teste de diferenças de comportamento (Timendus)
├── 6-keypad.ch8              # Teste de teclado (Timendus)
├── 7-beep.ch8                # Teste de som (Timendus)
├── 8-scrolling.ch8           # Teste de rolagem (Timendus)
├── lib/
│   ├── Emulador.h             # Estrutura, constantes, status e declarações
│   └── Emulador.c             # Estado, execução, disassembler e saída no terminal
└── src/
    ├── main.c                 # Carregamento da ROM e ciclo principal
    ├── emulador               # Executável Linux incluído
    └── emulador.exe           # Executável Windows incluído
```

As ROMs `6-keypad.ch8` e `7-beep.ch8` verificam recursos que ainda não estão implementados. As outras também podem depender de instruções ou detalhes de comportamento ausentes; a presença dos arquivos não significa que os testes já passem.

## Executar

Passe o caminho da ROM como único argumento. Execute os comandos a partir da pasta `src`, onde estão os binários; os exemplos usam uma ROM que fica um nível acima.

### Linux

```sh
./emulador ../IBM_Logo.ch8
```

Se necessário, permita a execução com `chmod +x emulador`. Para executar uma ROM de teste, por exemplo:

```sh
./emulador ../3-corax+.ch8
```

### Windows (PowerShell)

```powershell
.\emulador.exe ..\IBM_Logo.ch8
```

Para executar uma ROM de teste:

```powershell
.\emulador.exe ..\3-corax+.ch8
```

### Compilar

Com GCC instalado, compile a partir da pasta `src`.

Linux:

```sh
gcc -Wall -Wextra main.c ../lib/Emulador.c -o emulador
```

Windows, usando GCC (por exemplo, MinGW):

```powershell
gcc -Wall -Wextra main.c ..\lib\Emulador.c -o emulador.exe
```

## Disassembler

O [disassembler CHIP-8](https://github.com/otsuki1153/Disassembler-Model) é um projeto separado que lê uma ROM e traduz os opcodes em mnemônicos. A análise linear não diferencia código de dados, então bytes de sprites podem aparecer como instruções desconhecidas.

## Referências

- [Como escrever um interpretador CHIP-8 — Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Tabela de opcodes CHIP-8 — Gulrak](https://chip8.gulrak.net)
- [Suíte de testes CHIP-8 — Timendus](https://github.com/Timendus/chip8-test-suite)
- [Referência técnica CHIP-8 de Cowgod](https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod's%20CHIP-8%20Technical%20Reference.pdf)

## Licença

A definir.
