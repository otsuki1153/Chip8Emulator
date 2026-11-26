# CHIP-8 em C: [disassembler](https://github.com/otsuki1153/Disassembler-Model) e emulador

Projeto de estudo sobre emulação CHIP-8, escrito em C. O programa carrega uma ROM indicada na linha de comando, executa as instruções implementadas e desenha a tela em uma janela SDL3. O projeto de disassembler ligado no título é separado deste emulador.

## Estado do projeto

O núcleo já implementa o ciclo de busca, decodificação e execução, uma parte das instruções CHIP-8, a pilha de sub-rotinas e o desenho de sprites. O emulador ainda não tem teclado, temporizadores, som ou janela gráfica; ROMs que dependem desses recursos podem parar ao encontrar uma instrução não implementada.

### Funcionalidades e próximos passos

| Área | Implementado | Falta fazer |
|---|---|---|
| Máquina | Memória de 4 KB, registradores `V0`–`VF`, `I`, contador de programa e tela 64 × 32. | Completar a validação de limites e tratar falhas de leitura do contador de programa. |
| Pilha | Pilha de 16 posições; `CALL`/`RET` retornam erro de overflow/underflow. | — |
| Instruções | Conjunto listado na tabela abaixo, incluindo aritmética, saltos, desenho e operações de memória. | Implementar os opcodes restantes e conferir comportamentos de compatibilidade CHIP-8. |
| ROM | Recebe o caminho da ROM como argumento; aceita arquivos de até 3.584 bytes. | Melhorar mensagens e tratamento de erros de arquivo. |
| Tela | Desenha os pixels em uma janela SDL3. | Adicionar entrada de teclado. |
| Temporização e som | Limita o ciclo a cerca de 60 atualizações por segundo; detecta status de underflow dos timers. | Completar a temporização CHIP-8 e implementar som. |
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
5. A tela é atualizada na janela SDL3 ao longo da execução. Se o executor retornar uma instrução não implementada ou um erro de pilha/memória, o programa mostra a mensagem e encerra o ciclo.

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

Compile a partir da pasta `src`. O projeto usa SDL3 para abrir a janela gráfica, então inclua `display.c` e vincule a biblioteca SDL3.

Linux, com SDL3 e `pkg-config` instalados:

```sh
gcc -Wall -Wextra main.c ../lib/Emulador.c ../lib/display.c -o emulador $(pkg-config --cflags --libs sdl3)
```

Windows no CMD, com GCC e SDL3 do MSYS2 UCRT64:

```cmd
gcc -Wall -Wextra main.c ..\lib\Emulador.c ..\lib\display.c -IC:\msys64\ucrt64\include -LC:\msys64\ucrt64\lib -lSDL3 -o emulador.exe
```

Ajuste os caminhos de include e biblioteca se o MSYS2 estiver instalado em outro local. Execute o comando dentro de `src`.

## Disassembler

O [disassembler CHIP-8](https://github.com/otsuki1153/Disassembler-Model) é um projeto separado que lê uma ROM e traduz os opcodes em mnemônicos. A análise linear não diferencia código de dados, então bytes de sprites podem aparecer como instruções desconhecidas.

## Referências

- [Como escrever um interpretador CHIP-8 — Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [Tabela de opcodes CHIP-8 — Gulrak](https://chip8.gulrak.net)
- [Suíte de testes CHIP-8 — Timendus](https://github.com/Timendus/chip8-test-suite)
- [Referência técnica CHIP-8 de Cowgod](https://github.com/trapexit/chip-8_documentation/blob/master/Misc/Cowgod's%20CHIP-8%20Technical%20Reference.pdf)

## Licença

A definir.

## Configurações de execução

Ao iniciar, o programa solicita a configuração do emulador. Digite `1` para a configuração antiga ou `2` para a recente. Uma opção fora desse intervalo seleciona a configuração antiga. A ROM continua sendo informada como único argumento do programa; a configuração é escolhida interativamente depois que a ROM é aberta.

| Configuração | Instruções por ciclo | Comportamento após desenho |
|---|---:|---|
| Antiga | 50 | Interrompe o ciclo de instruções assim que executa `DXYN`. |
| Recente | 10 | Continua o ciclo até executar as instruções previstas ou ocorrer um erro. |

A configuração antiga também zera `VF` após as operações `8XY1`, `8XY2` e `8XY3` (OR, AND e XOR). Na configuração recente, essas operações preservam o valor de `VF`, exceto quando `VF` é o próprio registrador de destino.

## Desenho de sprites

A instrução `DXYN` limita as coordenadas iniciais aos limites da tela usando o resto da divisão por sua largura e altura (`64 × 32`). O sprite é desenhado pixel a pixel e as partes que ultrapassam as bordas direita ou inferior são cortadas; elas não reaparecem do outro lado. `VF` é zerado antes do desenho e passa a `1` quando um pixel já aceso é apagado, indicando colisão. A execução informa ao ciclo principal que um desenho foi realizado, o que encerra o ciclo da configuração antiga.

## Limites da configuração interativa

A seleção de modo é lida pelo terminal com `scanf`; o programa não recebe essa opção por argumento de linha de comando. A escolha determina quantas instruções são executadas por ciclo e alguns detalhes de compatibilidade, mas não implementa teclado físico ou som. Os ciclos também chamam a rotina de decremento dos temporizadores; isso não equivale a uma temporização calibrada em segundos ou à implementação de áudio.

## Referência técnica do núcleo

### Estado da máquina

A estrutura `Emulador`, declarada em `lib/Emulador.h`, reúne o estado completo mantido pelo núcleo:

| Campo | Tipo e dimensão | Uso |
|---|---|---|
| `memoria` | 4.096 bytes (`uint8_t`) | Memória endereçável de `0x000` a `0xFFF`. |
| `regV` | 16 bytes (`uint8_t`) | Registradores gerais `V0` a `VF`, com valores de 0 a 255. |
| `regI` | 16 bits (`uint16_t`) | Registrador de índice usado em endereços e operações com memória. |
| `sysCount` | 16 bits (`uint16_t`) | Contador de programa; inicia em `0x200`. |
| `pilha` | 16 endereços (`uint16_t`) | Endereços de retorno das sub-rotinas. |
| `posPilha` | 8 bits (`uint8_t`) | Próxima posição disponível na pilha. |
| `tela` | 32 × 64 valores booleanos | Buffer monocromático de pixels. |
| `teclas` | 16 valores booleanos | Estado lógico das teclas CHIP-8; o programa ainda não o alimenta por entrada física. |
| `DT`, `ST` | 8 bits cada | Registradores de delay e sound timer. |
| `legacy_mode` | booleano | Seleciona os detalhes de compatibilidade da configuração antiga. |

`initEmulator` inicializa o contador de programa em `START_ADDR` (`0x200`), zera `I`, pilha, registradores, memória, tela e teclas, e começa com o modo antigo desativado. O carregador aceita ROMs de até 3.584 bytes, que cabem no intervalo de `0x200` até o fim da memória (`0xFFF`). A rotina também escreve o valor `1` no endereço `0x1FF`; o núcleo atual não declara uma tabela de fontes nessa região.

### Busca e decodificação

Cada opcode CHIP-8 ocupa dois bytes. `fetch` combina o byte de maior endereço primeiro como byte alto (big-endian), retorna o opcode de 16 bits e avança `sysCount` em dois. `decode` divide esse valor em quatro nibbles de 4 bits, usados por `execute` para selecionar a operação e os registradores.

As rotinas de salto e de condição alteram o fluxo diretamente: `1NNN` e `BNNN` substituem o contador de programa; os saltos condicionais avançam mais dois bytes quando a condição é satisfeita. `2NNN` guarda o endereço da instrução seguinte na pilha antes de saltar. `00EE` recupera o endereço do topo. A pilha tem capacidade para 16 chamadas aninhadas.

### Lista detalhada de opcodes executados

`VX` e `VY` são registradores selecionados pelos dígitos `X` e `Y`; `NN` é um imediato de 8 bits; `NNN` é um endereço de 12 bits; `N` é a altura do sprite em linhas.

| Opcode | Operação implementada | Efeito principal |
|---|---|---|
| `00E0` | `CLS` | Limpa todos os pixels do buffer. |
| `00EE` | `RET` | Retira um endereço da pilha e o define como contador de programa. |
| `1NNN` | `JP NNN` | Salta para o endereço `NNN`. |
| `2NNN` | `CALL NNN` | Empilha o contador de programa atual e chama `NNN`. |
| `3XNN` | `SE VX, NN` | Pula uma instrução se `VX == NN`. |
| `4XNN` | `SNE VX, NN` | Pula uma instrução se `VX != NN`. |
| `5XY0` | `SE VX, VY` | Pula uma instrução se `VX == VY`; outros finais de `5XY*` não são implementados. |
| `6XNN` | `LD VX, NN` | Carrega o imediato em `VX`. |
| `7XNN` | `ADD VX, NN` | Soma o imediato a `VX`; o resultado é armazenado em 8 bits. |
| `8XY0` | `LD VX, VY` | Copia `VY` para `VX`. |
| `8XY1` | `OR VX, VY` | Armazena `VX OR VY` em `VX`. |
| `8XY2` | `AND VX, VY` | Armazena `VX AND VY` em `VX`. |
| `8XY3` | `XOR VX, VY` | Armazena `VX XOR VY` em `VX`. |
| `8XY4` | `ADD VX, VY` | Soma os valores; `VF` recebe 1 se houver carry, caso contrário 0. |
| `8XY5` | `SUB VX, VY` | Calcula `VX - VY`; `VF` recebe 1 se `VX >= VY`. |
| `8XY6` | `SHR VX, VY` | Copia `VY` para `VX`, desloca à direita e guarda o bit menos significativo em `VF`. |
| `8XY7` | `SUBN VX, VY` | Calcula `VY - VX`; `VF` recebe 1 se `VY >= VX`. |
| `8XYE` | `SHL VX, VY` | Copia `VY` para `VX`, desloca à esquerda e guarda o bit mais significativo original em `VF`. |
| `9XY0` | `SNE VX, VY` | Pula uma instrução se `VX != VY`; outros finais de `9XY*` não são implementados. |
| `ANNN` | `LD I, NNN` | Carrega o endereço em `I`. |
| `BNNN` | `JP V0, NNN` | Salta para `NNN + V0`. |
| `CXNN` | `RND VX, NN` | Gera um byte pseudoaleatório e aplica AND com `NN`. |
| `DXYN` | `DRW VX, VY, N` | Lê `N` bytes a partir de `I`, desenha sprites de 8 pixels de largura e atualiza a flag de colisão `VF`. |
| `EX9E` | `SKP VX` | Pula uma instrução se a tecla de índice `VX` estiver marcada como pressionada. |
| `EXA1` | `SKNP VX` | Pula uma instrução se a tecla de índice `VX` não estiver marcada como pressionada. |
| `FX07` | `LD VX, DT` | Copia o delay timer para `VX`. |
| `FX15` | `LD DT, VX` | Copia `VX` para o delay timer. |
| `FX18` | `LD ST, VX` | Copia `VX` para o sound timer; não há geração de som. |
| `FX1E` | `ADD I, VX` | Soma `VX` a `I`. |
| `FX33` | `LD B, VX` | Escreve centenas, dezenas e unidades decimais de `VX` em `I`, `I+1` e `I+2`. |
| `FX55` | `LD [I], VX` | Salva `V0` até `VX` na memória a partir de `I`. |
| `FX65` | `LD VX, [I]` | Carrega em `V0` até `VX` os bytes a partir de `I`. |

As operações `FX55` e `FX65` avançam `I` em `X + 1` depois da transferência. Os deslocamentos sempre copiam primeiro `VY` para `VX`, inclusive quando `X` e `Y` são iguais. Nas operações de soma e subtração que definem `VF`, o código escreve primeiro o resultado em `VX` e depois a flag em `VF`; portanto, quando `X == F`, a atualização final de `VF` prevalece.

### Instruções reconhecidas apenas pelo disassembler

`disassemble` formata mnemônicos para exibição, mas isso não significa que `execute` implemente a instrução. Por exemplo, `SYS NNN` é exibido para opcodes `0NNN`, porém o executor não implementa essa chamada e retorna `NOT_IMPLEMENTED`. O mesmo vale para `FX0A` (espera por tecla), `FX29` (endereço de fonte) e variantes de opcodes cujo padrão não aparece na tabela de instruções executadas. Ao encontrar uma dessas instruções, o ciclo imprime o opcode, o endereço e o mnemônico, marca erro e termina.

### Memória e erros de limite

As operações de sprite verificam se cada byte lido por `DXYN` cabe na memória. `FX33` verifica se os três bytes de saída cabem; `FX55` e `FX65` verificam cada byte da transferência. As falhas retornam `MEMORY_OUT_OF_BOUNDS`. Chamadas além das 16 posições da pilha retornam `STACK_OVERFLOW`; `RET` sem chamada anterior retorna `STACK_UNDERFLOW`. Índices de tecla fora do intervalo retornam `KEYBOARD_OUT_OF_BOUNDS`.

A busca verifica se há pelo menos dois bytes disponíveis antes de ler um opcode. Atualmente, quando essa verificação falha, `runEmulator` imprime uma mensagem, mas não transforma a falha em um status de erro; por isso o comportamento ao chegar ao fim da memória ainda precisa de tratamento. O laço também detecta uma instrução cujo contador de programa permanece igual ao valor anterior como tentativa de salto para si mesma e encerra a execução.

### Temporizadores, teclado e saída

O ciclo principal chama `decTimers` uma vez depois de cada lote de instruções: 50 instruções no modo antigo e 10 no recente. O ciclo é limitado a cerca de 60 atualizações por segundo. `decTimers` retorna um status se `DT` ou `ST` já estiverem em zero; os timers ainda não seguem a frequência CHIP-8 independente do ciclo. `FX07`, `FX15` e `FX18` permitem ler e escrever os campos, mas o timer de som não está ligado a uma saída de áudio.

As condições `EX9E` e `EXA1` consultam o vetor `teclas`, inicializado com todas as entradas falsas. Não há leitura de teclado no terminal ou janela para atualizar esse vetor. `FX0A`, que esperaria por uma tecla, retorna `NOT_IMPLEMENTED`.

A tela emulada é representada por uma matriz booleana de 64 colunas por 32 linhas e desenhada em uma janela SDL3. Ainda não há leitura de teclado pela janela.

### Detalhes do carregamento da ROM

O executável exige exatamente um argumento além do nome do programa: o caminho do arquivo ROM. Ele abre o arquivo em modo binário, mede seu tamanho, rejeita arquivos maiores que 3.584 bytes, retorna a posição de leitura ao início e copia os bytes para `memoria[0x200]`. O prompt de configuração aparece depois da abertura e da validação do tamanho do arquivo. Uma opção numérica diferente de `1` ou `2` seleciona o modo antigo; erros de leitura ou um arquivo inexistente encerram a inicialização com mensagem no terminal.

### Construção e dependências

O código usa a biblioteca padrão de C e depende da SDL3 para a janela gráfica. Os comandos de compilação acima partem da pasta `src` e compilam `main.c`, `../lib/Emulador.c` e `../lib/display.c`; os executáveis incluídos no repositório podem não refletir uma compilação mais recente do código-fonte.

### Escopo dos arquivos de teste

Os arquivos `1-chip8-logo.ch8`, `3-corax+.ch8`, `4-flags.ch8`, `5-quirks.ch8`, `6-keypad.ch8`, `7-beep.ch8` e `8-scrolling.ch8` são ROMs de referência para áreas diferentes: desenho e apresentação, opcodes, flags, diferenças de compatibilidade, teclado, som e rolagem. O conjunto presente no repositório não representa uma aprovação automática. Teclado, som, rolagem e alguns opcodes ainda não estão disponíveis, e os detalhes temporais e de compatibilidade não estão completos.
