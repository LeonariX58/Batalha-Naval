# Batalha Naval - Implementação Completa

Um jogo de Batalha Naval implementado em C com geração dinâmica de barcos e validação inteligente de tabuleiro.

## 📋 Descrição Completa do Código

### Visão Geral
Este projeto implementa um jogo clássico de Batalha Naval onde o jogador tenta acertar e afundar todos os navios inimigos dentro de um número limitado de rodadas. O código utiliza estruturas dinâmicas para flexibilidade e segurança.

### Configurações Principais
- **Tamanho do Tabuleiro**: 8x8 células (definido por `gridSize`)
- **Barcos Disponíveis**: 5 navios com tamanhos [2, 2, 3, 3, 5] células
- **Rodadas Permitidas**: (gridSize × gridSize) / 3 = aproximadamente 21 rodadas

## 🏗️ Estrutura do Código

### Estruturas de Dados

O código define três estruturas principais que organizam os dados do jogo:

**struct Position**: Representa um ponto (x, y) no tabuleiro. Facilita a passagem de coordenadas entre funções de forma limpa.

**struct Grid**: Contém duas matrizes 8x8 - `mesh` armazena a posição dos navios (0 = vazio, 1-5 = ID do navio) e `radar` rastreia os tiros do jogador (0 = não testado, 1 = água, 2 = acerto).

**struct Game**: Gerencia o estado completo do jogo com campos para maxRounds (número máximo permitido), round (rodada atual), state (0 = em andamento, 1 = vitória, -1 = derrota) e grid (o tabuleiro).

### Matrizes Globais Constantes

O código define duas arrays que controlam as direções possíveis para colocar navios: `dx[] = {1, -1, 0, 0}` e `dy[] = {0, 0, 1, -1}` representam movimento para direita, esquerda, baixo e cima.

A array `shipSize[] = {2, 2, 3, 3, 5}` define os tamanhos dos 5 navios. O valor `nShips` é calculado automaticamente usando `sizeof(shipSize) / sizeof(shipSize[0])` para obter a quantidade de elementos dinamicamente.

## 🔧 Funções e Seu Funcionamento

### genRand(int n)
Função simples que retorna um número aleatório entre 0 e n-1 usando `rand() % n`. É utilizada para gerar posições aleatórias dos navios e selecionar direções aleatoriamente.

### newPos(int x, int y)
Função auxiliar que cria e retorna uma struct Position com as coordenadas fornecidas. Melhora a legibilidade do código ao chamar funções que recebem posições.

### checkGridSize()
Valida se é possível colocar todos os barcos no tabuleiro. Faz duas verificações: confirma que nenhum navio individual é maior que o tabuleiro e verifica que a soma de todos os tamanhos dos navios não excede a área total do tabuleiro (64 células para um tabuleiro 8x8). Retorna 1 se válido, 0 caso contrário.

### tryNewShip(struct Grid grid, int rx, int ry, int rDir, int size)
Verifica se um navio de tamanho `size` pode ser colocado começando na posição (rx, ry) em uma direção específica `rDir`. Testa cada célula que o navio ocuparia para garantir que está dentro dos limites do tabuleiro e que nenhuma célula já está ocupada. Retorna 1 se a colocação é válida, 0 caso contrário.

### createNewGrid(struct Grid grid, int n)
Função recursiva que gera aleatoriamente a posição de todos os barcos. Para cada navio n (começando em 0), seleciona uma posição aleatória e tenta todas as 4 direções. Quando encontra uma direção válida, marca as células do navio na mesh e chama a si mesma com n+1. Se nenhuma direção funciona, tenta novamente com uma nova posição. Quando n == nShips, todos os barcos foram colocados e a função retorna o grid completo.

### compareGrid(struct Grid grid)
Verifica se o jogador venceu comparando o mesh (posição real dos navios) com o radar (tiros realizados). Percorre toda a grid e se encontra qualquer célula com um navio que não foi atingida (radar[i][j] != 2 enquanto mesh[i][j] != 0), retorna 0. Se todas as células com navios foram atingidas, retorna 1.

### placeGuess(struct Grid *grid, struct Position pos)
Registra um tiro do jogador. Se há um navio na posição especificada, define radar como 2 (acerto). Caso contrário, define como 1 (água). Usa ponteiro para evitar cópia desnecessária da struct Grid.

### updateGame(struct Game game, struct Position pos)
Atualiza o estado do jogo após um tiro. Chama placeGuess para registrar o tiro, incrementa o contador de rodadas e verifica as condições finais: se excedeu maxRounds, define state como -1 (derrota); se todos os navios foram atingidos, define state como 1 (vitória).

### drawGame(int grid[][gridSize])
Exibe uma matriz 8x8 no terminal. Imprime cada elemento separado por vírgula, com quebras de linha ao final de cada linha. Após imprimir o grid, mostra a mensagem "Digite a posicao de ataque."

### initGame()
Inicializa um novo jogo. Define maxRounds como (gridSize * gridSize) / 3 (21 rodadas para um tabuleiro 8x8) e chama createNewGrid para gerar aleatoriamente a posição dos navios no mesh.

## 🎮 Fluxo Principal do Jogo

A função main() inicia o jogo verificando se o tabuleiro permite colocar os barcos (usando checkGridSize). Se inválido, mostra mensagem de erro e encerra. Caso contrário, inicializa o jogo com initGame().

O loop principal continua enquanto game.state == 0 (em andamento). A cada iteração: exibe o radar (resultado dos tiros anteriores) com drawGame, lê as coordenadas do jogador, atualiza o jogo com updateGame, que incrementa as rodadas e verifica vitória/derrota.

Ao final do jogo (vitória com state=1 ou derrota com state=-1), o loop encerra e drawGame exibe o mesh (posição real dos navios) para revelar onde estavam os barcos.

## 💡 Decisões de Design Importantes

O código usa aritmética de ponteiros `sizeof(shipSize) / sizeof(shipSize[0])` para calcular automaticamente o número de barcos. Isso significa que se adicionar mais barcos à array shipSize, nShips se atualiza automaticamente sem precisar mudar código.

Usa ponteiros em funções como placeGuess para evitar cópias desnecessárias da struct Grid, que contém duas matrizes 8x8 (256 inteiros no total).

A geração de barcos usa recursão, que é elegante mas tem risco de stack overflow em configurações muito exigentes. Há um comentário TODO sugerindo refatoração para usar ponteiros.

A validação ocorre em duas camadas: checkGridSize() faz validação macro (há espaço total?) e tryNewShip() faz validação micro (posição específica é válida?).

## 🔍 Sistema de Estados e Coordenadas

O jogo possui três estados possíveis: 0 (em andamento - estado padrão), 1 (vitória quando todos os navios são atingidos), -1 (derrota quando excede maxRounds).

O sistema de coordenadas usa entrada 1-based (1 a 8) do jogador mas converte internamente para 0-based (0 a 7) usando `newPos(x - 1, y - 1)`.

O radar tem três valores possíveis: 0 (célula não testada), 1 (água - tiro sem efeito), 2 (acerto - atingiu um navio).

## ⚠️ Limitações Atuais

A função createNewGrid é recursiva e pode teoricamente causar stack overflow em tabuleiros muito grandes ou com muitos barcos. A função não protege contra múltiplos tiros na mesma célula (o jogador poderia desperdiçar rodadas). Não há salvamento de dados ou histórico de tiros para análise. A interface de entrada/saída é básica sem tratamento de erros para coordenadas inválidas ou entrada não numérica.

## 📝 Sugestões de Expansão

Possíveis melhorias incluem: implementar modo contra IA com estratégia de tiro, adicionar dificuldade variável ajustando número de rodadas, implementar modo dois jogadores alternado, adicionar validação robusta de entrada do usuário, melhorar visualização do tabuleiro com caracteres especiais, rastrear estatísticas de acertos/erros e tempo de jogo.

---

Compilação: `gcc battleship.c -o battleship`
Execução: `./battleship`
