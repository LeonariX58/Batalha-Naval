# ⚓ Batalha Naval - Implementação Completa em C

Um jogo de Batalha Naval implementado em C com geração dinâmica de barcos, validação inteligente de tabuleiro e sistema de jogo funcional e desafiador.

## 📋 Descrição do Projeto

Este projeto implementa um jogo clássico de Batalha Naval onde o jogador tenta acertar e afundar todos os navios inimigos dentro de um número limitado de rodadas. O código é totalmente modular, com estruturas de dados bem organizadas e funções específicas para cada aspecto da lógica do jogo.

### Características Principais
- ✅ Geração aleatória e válida de barcos no tabuleiro
- ✅ Sistema de validação em duas camadas (macro e micro)
- ✅ Cálculo dinâmico do número de barcos
- ✅ Interface de linha de comando simples
- ✅ Detecção automática de vitória/derrota
- ✅ Radar visual dos tiros realizados

### Configurações Padrão
- **Tamanho do Tabuleiro**: 8×8 células
- **Barcos**: 5 navios com tamanhos [2, 2, 3, 3, 5] células
- **Rodadas Permitidas**: 21 (aproximadamente 1/3 do tabuleiro)
- **Objetivo**: Afundar todos os 5 navios antes das rodadas acabarem

---

## 🏗️ Estrutura do Código

### Estruturas de Dados

O código define três estruturas principais:

#### **struct Position**
```c
struct Position {
    int x;
    int y;
};
```
Representa um ponto (x, y) no tabuleiro. Facilita a passagem de coordenadas entre funções de forma limpa e legível.

#### **struct Grid**
```c
struct Grid {
    int mesh[gridSize][gridSize];    // Posição real dos navios
    int radar[gridSize][gridSize];   // Resultado dos tiros
};
```
- **mesh**: Armazena a posição dos navios (0 = vazio, 1-5 = ID do navio)
- **radar**: Rastreia os tiros do jogador (0 = não testado, 1 = água, 2 = acerto)

#### **struct Game**
```c
struct Game {
    int maxRounds;    // Número máximo permitido de rodadas
    int round;        // Rodada atual
    int state;        // Estado do jogo (0 = jogando, 1 = vitória, -1 = derrota)
    struct Grid grid; // O tabuleiro do jogo
};
```

### Matrizes Globais Constantes

```c
static const int dx[] = {1, -1, 0, 0};   // Direções X: direita, esquerda, parado, parado
static const int dy[] = {0, 0, 1, -1};   // Direções Y: parado, parado, baixo, cima
static const int shipSize[] = {2, 2, 3, 3, 5};  // Tamanhos dos 5 navios
static const int nShips = sizeof(shipSize) / sizeof(shipSize[0]);  // = 5
```

Essas matrizes controlam:
- As 4 direções possíveis para colocar navios
- Os tamanhos de cada navio
- O número automático de navios (através de aritmética de sizeof)

---

## 🔧 Funções Explicadas

### `int genRand(int n)`
**Propósito**: Gerar números aleatórios de 0 a n-1

```c
int genRand(int n) {
    return rand() % n;
}
```
Função simples que retorna um número aleatório. Utilizada para:
- Gerar posições aleatórias dos navios
- Selecionar direções aleatoriamente

---

### `struct Position newPos(int x, int y)`
**Propósito**: Criar uma struct Position com coordenadas fornecidas

```c
struct Position newPos(int x, int y) {
    struct Position newPos;
    newPos.x = x; newPos.y = y;
    return newPos;
}
```
Função auxiliar que melhora a legibilidade ao chamar funções que recebem posições. Exemplo: `newPos(3, 5)` é mais legível que passar dois inteiros separados.

---

### `int checkGridSize()`
**Propósito**: Validar se é possível colocar todos os barcos no tabuleiro

```c
int checkGridSize() {
    int S = 0;
    for (int i = 0; i < nShips; i++) {
        if (shipSize[i] > gridSize) return 0;  // Navio maior que o tabuleiro?
        S += shipSize[i];
    }
    if (S > gridSize * gridSize) return 0;    // Soma de tamanhos > espaço?
    return 1;
}
```
Duas validações:
1. Nenhum navio individual é maior que o tabuleiro
2. A soma de todos os tamanhos cabe no tabuleiro

⚠️ **Nota**: Ainda é possível que combinações específicas de barcos não possam ser colocadas mesmo se essa função retornar 1.

---

### `int tryNewShip(struct Grid grid, int rx, int ry, int rDir, int size)`
**Propósito**: Testar se um navio pode ser colocado em uma posição/direção específica

```c
int tryNewShip(struct Grid grid, int rx, int ry, int rDir, int size) {
    for (int i = 0; i < size; i++) {
        int nextX = rx + (dx[rDir] * i);
        int nextY = ry + (dy[rDir] * i);
        
        if (nextX < 0 || nextX >= gridSize || 
            nextY < 0 || nextY >= gridSize || 
            grid.mesh[nextX][nextY]) {
            return 0;  // Fora dos limites ou já ocupado
        }
    }
    return 1;  // Posição válida
}
```
Testa cada célula que o navio ocuparia para garantir:
- Está dentro do tabuleiro
- Não sobrepõe outro navio

---

### `struct Grid createNewGrid(struct Grid grid, int n)`
**Propósito**: Gerar aleatoriamente a posição de todos os barcos (recursivo)

```c
struct Grid createNewGrid(struct Grid grid, int n) {
    if (n == nShips) return grid;  // Base: todos os navios foram colocados
    
    int rx = genRand(gridSize);
    int ry = genRand(gridSize);
    
    if (grid.mesh[rx][ry]) return createNewGrid(grid, n);  // Posição ocupada, tenta novamente
    
    int rDir = genRand(4);  // Tenta direção aleatória primeiro
    
    for (int i = 0; i < 4; i++) {
        int newDir = (rDir + i) % 4;
        
        if (tryNewShip(grid, rx, ry, newDir, shipSize[n])) {
            // Coloca o navio
            for (int j = 0; j < shipSize[n]; j++) {
                int nextX = rx + (dx[newDir] * j);
                int nextY = ry + (dy[newDir] * j);
                grid.mesh[nextX][nextY] = n + 1;  // 1-5 = ID do navio
            }
            return createNewGrid(grid, n + 1);  // Tenta colocar o próximo navio
        }
    }
    return createNewGrid(grid, n);  // Nenhuma direção funcionou, tenta de novo
}
```

**Lógica**:
1. Para cada navio n (0 a 4):
2. Seleciona posição aleatória (rx, ry)
3. Tenta direções começando pela aleatória
4. Quando encontra uma válida, marca no mesh e move para o próximo navio
5. Se nenhuma direção funciona, tenta novamente com outra posição

---

### `int compareGrid(struct Grid grid)`
**Propósito**: Verificar se todos os navios foram atingidos (vitória)

```c
int compareGrid(struct Grid grid) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid.radar[i][j] != 2 && grid.mesh[i][j]) return 0;
        }
    }
    return 1;
}
```
Percorre o tabuleiro. Se encontrar uma célula com navio (mesh != 0) que não foi atingida (radar != 2), retorna 0 (ainda há navios). Caso contrário, retorna 1 (vitória).

---

### `void placeGuess(struct Grid *grid, struct Position pos)`
**Propósito**: Registrar um tiro do jogador

```c
void placeGuess(struct Grid *grid, struct Position pos) {
    if (grid->mesh[pos.x][pos.y]) 
        grid->radar[pos.x][pos.y] = 2;  // Acerto
    else 
        grid->radar[pos.x][pos.y] = 1;  // Água
}
```
- Se há navio na posição: marca como 2 (acerto)
- Caso contrário: marca como 1 (água)
- Usa **ponteiro** para evitar cópia desnecessária da struct Grid

---

### `struct Game updateGame(struct Game game, struct Position pos)`
**Propósito**: Atualizar o estado do jogo após um tiro

```c
struct Game updateGame(struct Game game, struct Position pos) {
    placeGuess(&game.grid, pos);
    game.round++;
    
    if (game.round > game.maxRounds) game.state = -1;  // Derrota
    else if (compareGrid(game.grid)) game.state = 1;   // Vitória
    
    return game;
}
```
1. Registra o tiro
2. Incrementa contador de rodadas
3. Verifica condições finais

---

### `void drawGame(int grid[][gridSize])`
**Propósito**: Exibir o tabuleiro no terminal

```c
void drawGame(int grid[][gridSize]) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (j != gridSize - 1) 
                printf("%d, ", grid[i][j]); 
            else 
                printf("%d\n", grid[i][j]);
        }
    }
    printf("Digite a posicao de ataque.\n");
}
```
Imprime matriz 8×8 com formatação legível e instrução para o jogador.

---

### `struct Game initGame()`
**Propósito**: Inicializar um novo jogo

```c
struct Game initGame() {
    struct Game newGame = {0};
    newGame.maxRounds = (gridSize * gridSize) / 3;  // = 21
    newGame.grid = createNewGrid(newGame.grid, 0);
    return newGame;
}
```
- Define número máximo de rodadas
- Gera aleatoriamente a posição dos navios

---

## 🎮 Fluxo Principal (main)

```c
int main() {
    srand(time(NULL));  // Seed para RNG
    
    // Valida se é possível colocar barcos
    if (!checkGridSize()) {
        printf("Parametros dos barcos invalidos!");
        return 0;
    }
    
    struct Game game = initGame();
    
    do {
        drawGame(game.grid.radar);    // Mostra radar (tiros anteriores)
        int x, y;
        
        scanf("%d", &y);              // Lê coordenadas do jogador
        scanf("%d", &x);
        
        game = updateGame(game, newPos(x - 1, y - 1));  // Converte para 0-based
    } while (!game.state);            // Continua até vitória (1) ou derrota (-1)
    
    drawGame(game.grid.mesh);         // Revela posição real dos navios
    return 0;
}
```

**Fluxo**:
1. Inicializa RNG com seed baseado em tempo
2. Valida parâmetros
3. Cria novo jogo
4. **Loop principal**: exibe radar → lê posição → atualiza jogo
5. Quando game.state ≠ 0: mostra mesh e finaliza

---

## 💡 Decisões de Design

### Cálculo Dinâmico de Barcos
```c
static const int nShips = sizeof(shipSize) / sizeof(shipSize[0]);
```
Se adicionar mais barcos a `shipSize[]`, `nShips` se atualiza automaticamente.

### Uso de Ponteiros
Funções como `placeGuess` usam ponteiros para evitar copiar a struct Grid inteira (256 inteiros).

### Recursão na Geração
`createNewGrid()` é elegante mas pode causar stack overflow em configurações muito exigentes. Há um TODO sugerindo refatoração para iterativo.

### Validação em Duas Camadas
- **Macro**: `checkGridSize()` - há espaço total?
- **Micro**: `tryNewShip()` - posição específica é válida?

---

## 🔍 Sistema de Estados e Valores

### Estados do Jogo (`game.state`)
- **0**: Em andamento (estado padrão)
- **1**: Vitória (todos os navios atingidos)
- **-1**: Derrota (rodadas acabaram)

### Valores do Mesh
- **0**: Célula vazia
- **1-5**: ID do navio naquela posição

### Valores do Radar
- **0**: Célula não testada
- **1**: Água (tiro sem acerto)
- **2**: Acerto (atingiu um navio)

### Sistema de Coordenadas
- **Entrada**: 1-based (1 a 8) - mais intuitivo para o jogador
- **Interno**: 0-based (0 a 7) - conversão com `newPos(x-1, y-1)`

---

## ⚠️ Limitações Atuais

1. **Recursão**: `createNewGrid()` é recursiva e pode causar stack overflow em tabuleiros muito grandes
2. **Múltiplos Tiros**: Não protege contra tiros repetidos na mesma célula (problema de UX)
3. **Sem IA**: Jogo é contra o computador passivo (apenas distribui navios)
4. **Sem Validação de Entrada**: Não valida coordenadas inválidas do jogador

---

## 📝 Sugestões de Expansão

Possíveis melhorias:
- [ ] Modo contra IA com estratégia inteligente de tiro
- [ ] Dificuldade variável (ajustar número de rodadas)
- [ ] Modo dois jogadores alternado
- [ ] Proteção contra tiros repetidos
- [ ] Refatorar `createNewGrid()` para iterativo
- [ ] Sistema de pontuação/ranking
- [ ] Interface gráfica (SDL/OpenGL)
- [ ] Estatísticas de jogo (total de acertos, etc)

---

## 🛠️ Como Compilar e Executar

### Compilação
```bash
gcc battleship.c -o battleship
```

### Execução
```bash
./battleship
```

### Exemplo de Gameplay
```
0, 0, 0, 1, 0, 0, 0, 0
0, 0, 0, 1, 0, 0, 1, 0
0, 0, 0, 0, 0, 0, 1, 0
0, 0, 0, 0, 0, 0, 0, 0
0, 0, 0, 0, 0, 0, 0, 0
0, 1, 0, 0, 0, 0, 0, 0
0, 1, 0, 0, 0, 0, 0, 0
0, 1, 0, 0, 0, 0, 0, 0
Digite a posicao de ataque.
3 3
```

---

## 📚 Requisitos

- GCC (compilador C)
- Qualquer sistema operacional (Linux, macOS, Windows com MinGW)
- Conhecimento básico de C

---

## 📄 Licença

Uso livre para fins educacionais e pessoais.

---

**Autor**: LeonariX58  
**Última atualização**: Junho 2026
