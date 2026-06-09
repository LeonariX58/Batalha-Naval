#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Tamanho e estrutura/quantidade dos barcos é 100% dinâmico
// MAX: nAlphabet (26)
#define gridSize 8 // n x n 

// Declara como static const no escopo glboal por segurança e boa prática

static const int dx[] = {1, -1, 0, 0};
static const int dy[] = {0, 0, 1, -1};

static const char alphabet[] = "abcdefghijklmnopqrstuvwxyz"; // Para mostrar as coordenadas de forma mais amigável
static const int nAlphabet = sizeof(alphabet) - 1; // Vai que precisa mudar o alfabeto...
static const int shipSize[] = {2, 2, 2, 4, 6};
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

// Transforma o valor do radar em um char equivalente
char parseValue(int value) {
    if (value == 0) return '_';
    else if (value == 1) return 'O'; // Errou
    else if (value == 2) return 'X'; // Acertou
    else return '?';
}

int parseLetter(char c) {
    // Se for de 0 a 9 o char que o jogador colocou no lugar da letra o parse arruma
    // 0 = 48, 9 = 57
    if (c >= '0' && c <= '9') {
        return c - '0'; // Converte char para int (Ex. '3' - '0' = 3)
    }
    
    // A - Z = 65 - 90, a - z = 97 - 122 => c + 32 converte letra maiúscula para minúscula
    if (c >= 'A' && c <= 'Z') c = c + 32;

    for (int i = 0; i < nAlphabet; i++) {
        if (alphabet[i] == c) {
            return i;
        }
    }

    return -1; // letra inválida
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

// Função recursiva para gerar os barcos
struct Grid createNewGrid(struct Grid grid, int n) {
    if (n == nShips) return grid; // Solução trivial

    int rx; int ry;
    rx = genRand(gridSize);
    ry = genRand(gridSize);

    // Tecnicamente não precisa, pq já vai checar essa condição depois, mas deve poucar um pouquinho de processamento
    if (grid.mesh[rx][ry]) return createNewGrid(grid, n);
    
    int rDir = genRand(4); // Gera 4 possíveis direções, 0 - 3

    for (int i = 0; i < 4; i++) {
        // Garante que vai testar todas as direções
        int newDir = (rDir + i) % 4; // 1 + 0 % 4 = 1, 1 + 1 % 4 = 2, 1 + 2 % 4 = 3, 1 + 3 % 4 = 0

        if (tryNewShip(grid, rx, ry, newDir, n)) {
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
void drawGame(int grid[][gridSize], int type) {
    printf("   ");

    for (int i = 0; i < gridSize; i++) {
        printf("%02d ", i + 1); // Números menores que 10 ganham zero à esquerda
    }

    printf("\n");
    
    for (int i = 0; i < gridSize; i++) {
        if (i < nAlphabet) printf("%c: ", alphabet[i]);
        else printf("%d: ", (i - nAlphabet) + 1); // Se passar de 26 usa numeros
        for (int j = 0; j < gridSize; j++) {
            if (type) {
                if (j < gridSize - 1) printf("%c, ", parseValue(grid[i][j])); // _,_,_,X
                else printf("%c\n", parseValue(grid[i][j]));
            } else {
                if (j < gridSize - 1) printf("%d, ", grid[i][j]); // 0,0,1,0
                else printf("%d\n", grid[i][j]);
            }
        }
    }
}

struct Game initGame() {
    struct Game newGame = {0};
    // Permite que o jogador erre 33% dos ataques e ainda tenha o número de ataques igual ao número de espaços ocupados pelos navios
    newGame.maxRounds = ((gridSize * gridSize) / 3) + getShipSum();
    // newGame.maxRounds = 4; // Pra facilitar os testes
    
    // Mantenho sem usar ponteiros aqui para melhorar a legibilidade
    newGame.grid = createNewGrid(newGame.grid, 0);
    return newGame;
}

// Cuida do tratamento de entrada e garante uma posição válida do jogador
struct Position getPlayerPosition(struct Grid grid) {
    char charX;
    int y;
    int x;
    struct Position pos;

    do {
        // X e Y invertidos pela lógica de impressão que inverte linhas com colunas
        printf("Digite a coordenada X: (1-%d) ", gridSize);
        scanf("%d", &y);

        printf("Digite a coordenada Y: (a-%c) ", alphabet[gridSize - 1]);
        scanf(" %c", &charX);

        x = parseLetter(charX);
        pos = newPos(x, y - 1); 

        if (pos.valid != 1) {
            printf("Digite coordenadas validas!\n");
        } else if (grid.radar[pos.x][pos.y]) {
            printf("Digite coordenadas novas!\n");
        }
    } while (pos.valid != 1 || grid.radar[pos.x][pos.y]);

    return pos;
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
        drawGame(game.grid.radar, 1);

        struct Position pos = getPlayerPosition(game.grid);
        game = updateGame(game, pos);
    } while (!game.state); // game.state == 0

    drawGame(game.grid.radar, 1);
    
    if (game.state == -1) {
        printf("\nGame Over!\nPosicoes dos navios:\n");
        drawGame(game.grid.mesh, 0);
    } else printf("Parabens, voce venceu!\n");
    
    return 0;
}