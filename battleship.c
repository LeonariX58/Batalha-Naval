#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Tamanho e estrutura/quantidade dos barcos agora é 100% dinâmico
#define gridSize 8

// Declara como static const no escopo glboal por segurança e boa prática
static const int dx[] = {1, -1, 0, 0}; 
static const int dy[] = {0, 0, 1, -1};

static const int shipSize[] = {2, 2, 3, 3, 5};
static const int nShips = sizeof(shipSize) / sizeof(shipSize[0]); // Mágica para ter o tamanho da array dinâmico
// *Pega o tamanho total da array em bits e divide pelo tamanho do elemento em bytes para retornar a quantidade de elementos

struct Position {
    int x;
    int y;
};

struct Grid {
    int mesh[gridSize][gridSize];
    int radar[gridSize][gridSize]; 
};

struct Game {
    int maxRounds;
    int round;
    int state;
    struct Grid grid;
};

// Gera um número aleatório de 0 - (n-1)
int genRand(int n) {
    return rand() % n;
}

// Esse struct só existe pra facilitar passar coordenadas nas funções
struct Position newPos(int x, int y) {
    struct Position newPos;
    newPos.x = x; newPos.y = y;

    return newPos;
}

// Teste simples se o tabuleiro não ta pequeno demais pros barcos
// **Ainda é possivel que existam combinações de barcos que não podem existir no tabuleiro mesmo se essa função retornar 1
int checkGridSize() {
    int S = 0;
    for (int i = 0; i < nShips; i++) {
        if (shipSize[i] > gridSize) return 0;
        S += shipSize[i];
    }

    if (S > gridSize * gridSize) return 0;
    return 1;
}
// Testa se os espaços naquela direção estão disponíveis
int tryNewShip(struct Grid grid, int rx, int ry, int rDir, int size) {
    for (int i = 0; i < size; i++) {
        int nextX = rx + (dx[rDir] * i);
        int nextY = ry + (dy[rDir] * i);

        if (nextX < 0 || nextX >= gridSize || nextY < 0 || nextY >= gridSize || grid.mesh[nextX][nextY]) {
            return 0;
        }
    }
    return 1;
}

// TODO: Talvez fazer ela funcionar com ponteiro ao invés de copiar a struct
// Função recursiva para gerar os barcos
struct Grid createNewGrid(struct Grid grid, int n) {
    if (n == nShips) return grid; // Solução trivial

    int rx; int ry;
    rx = genRand(gridSize);
    ry = genRand(gridSize);

    // Tecnicamente não precisa, pq já vai checar essa condição depois, mas deve poucar um pouquinho de processamento
    if (grid.mesh[rx][ry]) return createNewGrid(grid, n);
    
    int rDir = genRand(4); // Gera 4 possíveis direções
    
    for (int i = 0; i < 4; i++) {
        // Garante que vai testar todas as direções
        int newDir = (rDir + i) % 4;

        if (tryNewShip(grid, rx, ry, newDir, shipSize[n])) {
            for (int j = 0; j < shipSize[n]; j++) {
                int nextX = rx + (dx[newDir] * j);
                int nextY = ry + (dy[newDir] * j);
                grid.mesh[nextX][nextY] = n + 1;
            }
            return createNewGrid(grid, (n + 1)); // Faz de novo, mas com o grid atualizado e o n + 1
        }
    }
    return createNewGrid(grid, n); // Tenta de novo se não achou direção válida
}

// Retorna 1 se os navios foram todos atingidos e 0 caso contrário
int compareGrid(struct Grid grid) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (grid.radar[i][j] != 2 && grid.mesh[i][j]) return 0;
        }
    }
    return 1;
}

// Usa ponteiro aqui pra evitar ficar clonando struct atoa
void placeGuess(struct Grid *grid, struct Position pos) {
    if (grid->mesh[pos.x][pos.y]) grid->radar[pos.x][pos.y] = 2;
    else grid->radar[pos.x][pos.y] = 1;
}

struct Game updateGame(struct Game game, struct Position pos) {
    placeGuess(&game.grid, pos);
    game.round++;

    if (game.round > game.maxRounds) game.state = -1;
    else if (compareGrid(game.grid)) game.state = 1;

    return game;
}

// Desenha uma array de tamanho gridSize x gridSize no terminal
void drawGame(int grid[][gridSize]) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (j != gridSize - 1) printf("%d, ", grid[i][j]); 
            else printf("%d\n", grid[i][j]); // Se tiver no ultimo elemento da fileira pula linha
        }
    }
    printf("Digite a posicao de ataque.\n");
}

struct Game initGame() {
    struct Game newGame = {0};
    newGame.maxRounds = (gridSize * gridSize) / 3; // Da pra chutar até 1 terço do tabuleiro
    newGame.grid = createNewGrid(newGame.grid, 0);
    // Se eu fizer aquela nova implementação vai ficar só:
    // createNewGrid(newGame.grid, 0);
    return newGame;
}

int main() {
    srand(time(NULL));

    // Previni o código de rodar se não for possível colocar os barcos no tabuleiro
    if (!checkGridSize()) {
        printf("Parametros dos barcos invalidos!");
        return 0;
    }

    struct Game game = initGame();

    do {
        drawGame(game.grid.radar);
        int x; int y;

        scanf("%d", &y);
        scanf("%d", &x);


        game = updateGame(game, newPos(x - 1, y - 1));
    } while (!game.state);
    
    drawGame(game.grid.mesh); // Mostra como era o jogo (Temporário)
    return 0;
}