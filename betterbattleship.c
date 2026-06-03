// Versão teste com o objetivo de corrigir os problemas de performance do código original, mesmo perdendo legibilidade
// Este código não deve ser usado na apresentação final, mas sim como um exemplo de implementação mais otimizada
// Provavelmente nem vou mexer nesse treco
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Tamanho e estrutura/quantidade dos barcos é 100% dinâmico
#define gridSize 8 // n x n

// Declara como static const no escopo glboal por segurança e boa prática
static const int dx[] = {1, -1, 0, 0}; 
static const int dy[] = {0, 0, 1, -1};

static const int shipSize[] = {2, 2, 3, 3, 5};
static const int nShips = sizeof(shipSize) / sizeof(shipSize[0]); // Mágica para ter o tamanho da array dinâmico
// *Pega o tamanho total da array em bits e divide pelo tamanho do elemento em bytes para retornar a quantidade de elementos

struct Position {
    int x;
    int y;
    int valid;
};

struct Grid {
    int mesh[gridSize][gridSize]; // Mapa dos barcos
    int radar[gridSize][gridSize]; // Mapa do player
};

struct Game {
    int maxRounds;
    int round;
    int state; // -1 = Game Over, 0 = Em andamento, 1 = Venceu
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

    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) newPos.valid = 0;
    else newPos.valid = 1;

    return newPos;
}

int getShipSum() {
    int sum = 0;
    for (int i = 0; i < nShips; i++) {
        sum += shipSize[i];
    }
    return sum;
}

// Teste simples se o tabuleiro não ta pequeno demais pros barcos
// **Ainda é possivel que existam combinações de barcos que não podem existir no tabuleiro mesmo se essa função retornar 1
int checkGridSize() {
    for (int i = 0; i < nShips; i++) {
        if (shipSize[i] > gridSize) return 0;
    }

    if (getShipSum() > gridSize * gridSize) return 0;
    return 1;
}

// Testa se os espaços naquela direção estão disponíveis
int tryNewShip(struct Grid grid, int x, int y, int dir, int n) {
    for (int i = 0; i < shipSize[n]; i++) {
        int nextX = x + (dx[dir] * i);
        int nextY = y + (dy[dir] * i);

        // Restringe o próximo espaço a ser colocado dentro do tabuleiro e que ele não esteja ocupado por outro barco
        if (nextX < 0 || nextX >= gridSize || nextY < 0 || nextY >= gridSize || grid.mesh[nextX][nextY]) {
            return 0;
        }
    }
    return 1;
}

// TODO: Talvez fazer ela funcionar com ponteiro ao invés de copiar a struct
// Função recursiva para gerar os barcos
void createNewGrid(struct Grid *grid, int n) {
    if (n == nShips) return;

    int rx; int ry;
    rx = genRand(gridSize);
    ry = genRand(gridSize);

    // Tecnicamente não precisa, pq já vai checar essa condição depois, mas deve poucar um pouquinho de processamento
    if (grid->mesh[rx][ry]) return createNewGrid(grid, n);
    
    int rDir = genRand(4); // Gera 4 possíveis direções, 0 - 3

    for (int i = 0; i < 4; i++) {
        // Garante que vai testar todas as direções
        int newDir = (rDir + i) % 4; // 1 + 0 % 4 = 1, 1 + 1 % 4 = 2, 1 + 2 % 4 = 3, 1 + 3 % 4 = 0

        if (tryNewShip(*grid, rx, ry, newDir, n)) {
            for (int j = 0; j < shipSize[n]; j++) {
                int nextX = rx + (dx[newDir] * j);
                int nextY = ry + (dy[newDir] * j);
                grid->mesh[nextX][nextY] = n + 1; 
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

// TODO: colocar um \n entre os For pra ficar mais legível
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
    // Permite que o jogador erre 25% dos ataques e ainda tenha o número de ataques igual ao número de espaços ocupados pelos navios
    newGame.maxRounds = ((gridSize * gridSize) / 4) + getShipSum();
    createNewGrid(&newGame.grid, 0);
    return newGame;
}

int main() {
    srand(time(NULL));

    // Previni o código de rodar se não for possível colocar os barcos no tabuleiro
    if (!checkGridSize()) {
        printf("Parametros dos barcos invalidos!\n");
        return 0;
    }

    struct Game game = initGame();

    do {
        drawGame(game.grid.mesh);

        int x; int y;

        scanf("%d", &y);
        scanf("%d", &x);
        struct Position pos = newPos(x - 1, y - 1);
        
        // Garante que o jogador digite coordenadas válidas e que ele não tenha atacado aquela posição antes
        // Força o jogador a digitar novas coordenadas caso ele tenha digitado algo inválido ou que ele já tenha atacado antes
        while (pos.valid != 1 || game.grid.radar[pos.x][pos.y]) {
            if (game.grid.radar[pos.x][pos.y]) printf("Digite coordenadas novas!\n");
            else printf("Digite coordenadas validas!\n");
            
            scanf("%d", &y);
            scanf("%d", &x);
            pos = newPos(x - 1, y - 1); 
        }

        // Apenas atualiza o jogo se as coordenadas forem válidas e que ele não tenha atacado aquela posição antes
        game = updateGame(game, pos);
    } while (!game.state);

    drawGame(game.grid.mesh); // Mostra como era o jogo (Temporário)
    return 0;
}