#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define gridSize 10
#define nShips 5

// Declara como static const no escopo glboal por segurança e boa prática
static const int dx[] = {1, -1, 0, 0}; 
static const int dy[] = {0, 0, 1, -1};

struct Grid {
    int mesh[gridSize][gridSize];
    int radar[gridSize][gridSize];
};

struct Game {
    int maxRounds;
    int round;
    int shipSize[nShips];
    struct Grid grid;
};

// Gera um número aleatório de 0 - (n-1)
int genRand(int n) {
    return rand() % n;
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

// Função recursiva para gerar os barcos
struct Grid createNewGrid(struct Grid grid, int *shipSize, int n) {
    if (n == nShips) return grid; // Condição trivial

    int rx; int ry;
    rx = genRand(gridSize);
    ry = genRand(gridSize);

    // Tecnicamente não precisa, pq já vai checar essa condição depois, mas deve poucar um pouquinho de processamento
    if (grid.mesh[rx][ry]) return createNewGrid(grid, shipSize, n);
    
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
            return createNewGrid(grid, shipSize, (n + 1)); // Faz de novo, mas com o grid atualizado e o n + 1
        }
    }
    return createNewGrid(grid, shipSize, n); // Tenta de novo se não achou direção válida
}

void setShips(int *shipSize) {
    int tempShipSize[nShips] = {2, 2, 3, 3, 5};
    for (int i = 0; i < nShips; i++) {
        shipSize[i] = tempShipSize[i];
    }
}

void drawGameDev(struct Game game) {
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (j != gridSize - 1) printf("%d, ", game.grid.mesh[i][j]);
            else printf("%d\n", game.grid.mesh[i][j]);
        }
    }
}

struct Game initGame() {
    struct Game newGame = {0};

    newGame.maxRounds = 30;
    setShips(newGame.shipSize);
    newGame.grid = createNewGrid(newGame.grid, newGame.shipSize, 0);

    return newGame;
}

int main() {
    srand(time(NULL));

    struct Game game = initGame();

    drawGameDev(game);
    return 0;
}