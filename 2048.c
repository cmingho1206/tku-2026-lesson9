#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct {
    int **grid;
    int size;
    int score;
} Game2048;

// 初始化遊戲
Game2048* init_game(int size) {
    Game2048 *game = malloc(sizeof(Game2048));
    game->size = size;
    game->score = 0;
    game->grid = malloc(size * sizeof(int *));
    for (int i = 0; i < size; i++) {
        game->grid[i] = calloc(size, sizeof(int)); // 初始化為 0
    }
    return game;
}

// 隨機在空格處生成數字
void spawn_number(Game2048 *game) {
    int count = 0;
    for (int i = 0; i < game->size; i++)
        for (int j = 0; j < game->size; j++)
            if (game->grid[i][j] == 0) count++;

    if (count == 0) return;

    int r = rand() % count;
    int current = 0;
    for (int i = 0; i < game->size; i++) {
        for (int j = 0; j < game->size; j++) {
            if (game->grid[i][j] == 0) {
                if (current == r) {
                    game->grid[i][j] = (rand() % 10 == 0) ? 4 : 2;
                    return;
                }
                current++;
            }
        }
    }
}

// 核心處理：壓縮與合併單一行列
bool slide_and_merge(int *line, int size, int *score) {
    bool moved = false;
    int *new_line = calloc(size, sizeof(int));
    int pos = 0;

    // 1. 壓縮：先移動非零數字
    for (int i = 0; i < size; i++) {
        if (line[i] != 0) {
            new_line[pos++] = line[i];
        }
    }

    // 2. 合併
    for (int i = 0; i < pos - 1; i++) {
        if (new_line[i] != 0 && new_line[i] == new_line[i+1]) {
            new_line[i] *= 2;
            *score += new_line[i];
            new_line[i+1] = 0;
            moved = true;
        }
    }

    // 3. 再次壓縮 (合併後產生的空格)
    int *final_line = calloc(size, sizeof(int));
    int final_pos = 0;
    for (int i = 0; i < size; i++) {
        if (new_line[i] != 0) final_line[final_pos++] = new_line[i];
    }

    // 檢查是否有變動
    for (int i = 0; i < size; i++) {
        if (line[i] != final_line[i]) moved = true;
        line[i] = final_line[i];
    }

    free(new_line);
    free(final_line);
    return moved;
}

// 根據方向移動 (0:上, 1:下, 2:左, 3:右)
bool move_board(Game2048 *game, int dir) {
    bool moved = false;
    int size = game->size;
    int *temp = malloc(size * sizeof(int));

    for (int i = 0; i < size; i++) {
        // 提取需要處理的線
        for (int j = 0; j < size; j++) {
            if (dir == 0) temp[j] = game->grid[j][i];      // Up
            else if (dir == 1) temp[j] = game->grid[size-1-j][i]; // Down
            else if (dir == 2) temp[j] = game->grid[i][j];      // Left
            else if (dir == 3) temp[j] = game->grid[i][size-1-j]; // Right
        }

        if (slide_and_merge(temp, size, &game->score)) moved = true;

        // 寫回網格
        for (int j = 0; j < size; j++) {
            if (dir == 0) game->grid[j][i] = temp[j];
            else if (dir == 1) game->grid[size-1-j][i] = temp[j];
            else if (dir == 2) game->grid[i][j] = temp[j];
            else if (dir == 3) game->grid[i][size-1-j] = temp[j];
        }
    }
    free(temp);
    return moved;
}

void print_ui(Game2048 *game) {
    system("clear || cls"); // 清除螢幕
    printf("Score: %d\n", game->score);
    for (int i = 0; i < game->size; i++) {
        for (int j = 0; j < game->size; j++) {
            if (game->grid[i][j] == 0) printf(". \t");
            else printf("%d\t", game->grid[i][j]);
        }
        printf("\n\n");
    }
    printf("WASD to move, Q to quit: ");
}

int main() {
    srand(time(NULL));
    int size = 4;
    Game2048 *game = init_game(size);
    spawn_number(game);
    spawn_number(game);

    char c;
    while (true) {
        print_ui(game);
        scanf(" %c", &c);
        int dir = -1;
        if (c == 'w' || c == 'W') dir = 0;
        else if (c == 's' || c == 'S') dir = 1;
        else if (c == 'a' || c == 'A') dir = 2;
        else if (c == 'd' || c == 'D') dir = 3;
        else if (c == 'q' || c == 'Q') break;

        if (dir != -1) {
            if (move_board(game, dir)) {
                spawn_number(game);
            }
        }
    }

    // 釋放記憶體
    for (int i = 0; i < size; i++) free(game->grid[i]);
    free(game->grid);
    free(game);
    return 0;
}