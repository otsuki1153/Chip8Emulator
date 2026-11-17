# Boas Práticas de Arquitetura e Desenvolvimento de Emuladores

Este guia reúne padrões arquiteturais, convenções e práticas de engenharia de software amplamente adotadas pela comunidade de desenvolvimento de emulação (Emudev), aplicáveis a qualquer sistema (CHIP-8, Game Boy, NES, etc.).

---

## 1. Fragmentação da Lógica e Separação de Responsabilidades

Emuladores crescem rapidamente em complexidade. Manter toda a lógica concentrada em um único laço ou arquivo dificulta a depuração e a portabilidade.

### O Ciclo Fetch-Decode-Execute
Divida o passo principal da CPU em três etapas claras e independentes:
* **Fetch (Busca):** Lê o próximo opcode da memória usando o Program Counter (`PC`) e avança o ponteiro.
* **Decode (Decodificação):** Extrai os operandos e identifica qual operação deve ser realizada, sem alterar o estado da máquina.
* **Execute (Execução):** Modifica registradores, flags, memória ou temporizadores com base no que foi decodificado.

### Desacoplamento entre Núcleo (Core) e Plataforma (Frontend)
O núcleo do emulador nunca deve depender diretamente de bibliotecas gráficas, de áudio ou de entrada (como SDL, Raylib ou Win32).
* **Core (Backend):** Contém apenas lógica pura (CPU, memória, barramentos, timers). Recebe comandos abstratos (ex: estado dos botões) e expõe buffers de dados (ex: matriz de pixels e amostras de áudio).
* **Frontend:** Responsável por criar janelas, renderizar o framebuffer na tela, reproduzir áudio e capturar teclas do usuário. Isso permite portar o emulador para outras plataformas ou rodar testes automatizados sem interface gráfica.

### Organização Modular por Subsistemas
Estruture o projeto espelhando os componentes físicos do hardware original:
* `cpu.c / cpu.h`: Registradores, ciclo de instrução e execução de opcodes.
* `mmu.c / mmu.h` (ou `bus.c`): Gerenciamento de memória, mapeamento de endereços e leitura/escrita.
* `ppu.c / ppu.h` (ou `display.c`): Processamento gráfico e manipulação do framebuffer.
* `apu.c / apu.h` (ou `audio.c`): Síntese sonora e temporizadores.

### Tabelas de Despacho (Dispatch Tables)
Evite cadeias extensas e aninhadas de `if / else if` ou `switch` gigantescos para decodificar instruções.
* Utilize **arrays de ponteiros de função** indexados pelos bits mais significativos do opcode.
* Isso melhora a legibilidade, reduz a complexidade ciclomática e facilita a localização de cada instrução no código-fonte.

---

## 2. Nomenclatura Padrão da Comunidade (Emudev)

Adotar o vocabulário clássico da literatura de arquitetura de computadores e da documentação técnica torna o código autoexplicativo para outros desenvolvedores e facilita a consulta a manuais de referência.

### Registradores e Ponteiros de Controle
| Termo Padrão | Significado | Evitar |
| :--- | :--- | :--- |
| **`PC`** | *Program Counter* (aponta para a próxima instrução na memória) | `ponteiro_codigo`, `linha_atual` |
| **`SP`** | *Stack Pointer* (aponta para o topo da pilha de chamadas) | `indice_pilha`, `topo` |
| **`I`** | *Index Register* (registrador de endereço de memória) | `reg_endereco`, `ponteiro_mem` |
| **`V0` – `VF`** | Registradores de propósito geral e flag de status (`VF`) | `regV[0]`, `variaveis[]` |
| **`DT` / `ST`** | *Delay Timer* e *Sound Timer* | `tempo1`, `contador_som` |

### Componentes de Instrução (Operandos)
Em documentações de arquitetura, os bits de uma instrução são representados por letras padronizadas. Use-as como nomes de variáveis locais na decodificação:
* **`opcode`**: A instrução bruta completa lida da memória (ex: `0x8124`).
* **`nnn` ou `addr`**: Endereço de memória de 12 bits contido nos bits inferiores.
* **`n` ou `nibble`**: Valor imediato de 4 bits (geralmente os últimos 4 bits).
* **`x` e `y`**: Índices de 4 bits que identificam os registradores envolvidos (`Vx` e `Vy`).
* **`kk` ou `byte`**: Constante imediata de 8 bits.

### Estruturas de Hardware
* **`bus` / `mmu`**: Barramento ou Unidade de Gerenciamento de Memória.
* **`ram` / `rom`**: Memória volátil e memória somente leitura carregada do cartucho/arquivo.
* **`stack`**: Pilha exclusiva para endereços de retorno de sub-rotinas.
* **`framebuffer` / `vram`**: Matriz que armazena o estado atual dos pixels da tela.
* **`keypad` / `joypad`**: Estado atual das teclas ou botões mapeados do console.

---

## 3. Boas Práticas Gerais de Implementação

### Tipos de Dados com Largura Fixa
Nunca dependa de tipos genéricos como `int` ou `long`, pois seu tamanho varia conforme o compilador e o sistema operacional.
* Use sempre os tipos da biblioteca `<stdint.h>`: `uint8_t` (bytes), `uint16_t` (endereços e opcodes de 16 bits) e `uint32_t` (pixels RGBA e contadores de ciclos).

### Operações Bitwise Explícitas
Emulação lida diretamente com manipulação de bits. Prefira máscaras hexadecimais e deslocamentos claros em vez de divisões ou módulos matemáticos para extrair dados.
* **Exemplo claro:** `uint8_t x = (opcode & 0x0F00) >> 8;`
* **Exemplo claro:** `uint8_t kk = opcode & 0x00FF;`

### Estado Encapsulado em Structs (Sem Variáveis Globais)
Agrupe todo o estado do hardware em uma única `struct` principal (ex: `typedef struct { ... } Chip8;`) passada por ponteiro para as funções.
* Isso permite instanciar múltiplas máquinas simultaneamente, facilita a implementação de recursos como *Save States* (basta serializar a struct) e evita efeitos colaterais ocultos.

### Separação entre Execução e Disassembler
Mantenha a função que traduz opcodes em texto legível (ex: `"ADD V1, V2"`) completamente separada da função que executa a instrução.
* O **Disassembler** deve apenas ler a memória sem alterar nenhum registrador ou flag, permitindo inspecionar o código em tempo real através de um depurador visual.

### Determinismo e Controle de Tempo
Não atrele a velocidade da CPU à taxa de quadros (FPS) do monitor de forma implícita.
* Execute um número fixo de ciclos de CPU por frame (ex: 10 instruções por frame a 60 Hz = 600 Hz) e atualize os temporizadores (`DT` e `ST`) estritamente a 60 Hz.
* Evite o uso de números aleatórios não determinísticos sem suporte a *seed* fixa, pois isso dificulta a reprodução de bugs.
