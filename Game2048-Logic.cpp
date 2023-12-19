#include "Game2048.h"

/* 随机放置新数字 */
void Game2048::putNumber() {
    // 随机生成0~3的整数
    std::uniform_int_distribution<int> randNum(0, 3);
    int x = randNum(randGen);
    int y = randNum(randGen);
    // 随机挑选一个空格子放置新数字
    while (board[x][y] != 0) {
        x = randNum(randGen);
        y = randNum(randGen);
    }
    // 10%的概率生成4，90%的概率生成2
    board[x][y] = (randGen() % 10 == 0) ? 4 : 2;
}

/* 初始化方格数据 */
void Game2048::initBoard() {
    // 清空方格数据，并放置两个初始数字
    memset(board, 0, sizeof(board));
    putNumber();
    putNumber();

    // 设置最高分并重置当前分数
    bestScore = max(bestScore, score);
    score = 0;
}

/* 向上移动 */
void Game2048::moveUp() {
    bool moved = false;
    for (int col = 0; col < 4; col++) {
        // 最顶部的一行不需要移动，所以从第二行开始遍历
        for (int row = 1; row < 4; row++) {
            if (board[row][col] == 0) continue;
            int preRow = row - 1;
            while (preRow >= 0 && board[preRow][col] == 0) preRow--;
            // 如果上面全是空行
            if (preRow == -1) {
                board[0][col] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
            // 如果上面的数字相同可以合并
            else if (board[preRow][col] == board[row][col]) {
                board[preRow][col] *= 2;
                score += board[preRow][col];
                board[row][col] = 0;
                moved = true;
            }
            // 如果上面有空行
            else if (preRow + 1 != row) {
                board[preRow + 1][col] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
        }
    }
    // 如果是有效移动，就放置新数字
    if (moved) putNumber();
    checkDead();  // 检查游戏是否失败
    checkWin();   // 检查游戏是否胜利
}

/* 向下移动 */
void Game2048::moveDown() {
    bool moved = false;
    for (int col = 0; col < 4; col++) {
        // 最底部的一行不需要移动，所以从倒数第二行开始遍历
        for (int row = 2; row >= 0; row--) {
            if (board[row][col] == 0) continue;
            int preRow = row + 1;
            while (preRow < 4 && board[preRow][col] == 0) preRow++;
            // 如果下面全是空行
            if (preRow == 4) {
                board[3][col] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
            // 如果下面的数字相同可以合并
            else if (board[preRow][col] == board[row][col]) {
                board[preRow][col] *= 2;
                score += board[preRow][col];
                board[row][col] = 0;
                moved = true;
            }
            // 如果下面有空行
            else if (preRow - 1 != row) {
                board[preRow - 1][col] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
        }
    }
    if (moved) putNumber();
    checkDead();
    checkWin();
}

/* 向左移动 */
void Game2048::moveLeft() {
    bool moved = false;
    for (int row = 0; row < 4; row++) {
        // 最左边的一列不需要移动，所以从第二列开始遍历
        for (int col = 1; col < 4; col++) {
            if (board[row][col] == 0) continue;
            int preCol = col - 1;
            while (preCol >= 0 && board[row][preCol] == 0) preCol--;
            // 如果左边全是空列
            if (preCol == -1) {
                board[row][0] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
            // 如果左边的数字相同可以合并
            else if (board[row][preCol] == board[row][col]) {
                board[row][preCol] *= 2;
                score += board[row][preCol];
                board[row][col] = 0;
                moved = true;
            }
            // 如果左边有空列
            else if (preCol + 1 != col) {
                board[row][preCol + 1] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
        }
    }
    if (moved) putNumber();
    checkDead();
    checkWin();
}

/* 向右移动 */
void Game2048::moveRight() {
    bool moved = false;
    for (int row = 0; row < 4; row++) {
        // 最右边的一列不需要移动，所以从倒数第二列开始遍历
        for (int col = 2; col >= 0; col--) {
            if (board[row][col] == 0) continue;
            int preCol = col + 1;
            while (preCol < 4 && board[row][preCol] == 0) preCol++;
            // 如果右边全是空列
            if (preCol == 4) {
                board[row][3] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
            // 如果右边的数字相同可以合并
            else if (board[row][preCol] == board[row][col]) {
                board[row][preCol] *= 2;
                score += board[row][preCol];
                board[row][col] = 0;
                moved = true;
            }
            // 如果右边有空列
            else if (preCol - 1 != col) {
                board[row][preCol - 1] = board[row][col];
                board[row][col] = 0;
                moved = true;
            }
        }
    }
    if (moved) putNumber();
    checkDead();
    checkWin();
}

// 检查是否胜利
void Game2048::checkWin() {
    if (GameState == UI_INFINITY) return;
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            if (board[row][col] == 2048) GameState = UI_WIN;
}

// 检查是否失败
void Game2048::checkDead() {
    bool dead = true;
    for (int row = 0; row < 4; row++) {
        for (int col = 1; col < 4; col++) {
            // 如果有空格或者相邻的格子数字相同，那么游戏还没有结束
            if (board[row][col] == 0 || board[row][col - 1] == 0 || board[row][col] == board[row][col - 1]) {
                dead = false;
                break;
            }
        }
        if (!dead) break;
    }
    for (int col = 0; col < 4; col++) {
        for (int row = 1; row < 4; row++) {
            // 如果有空格或者相邻的格子数字相同，那么游戏还没有结束
            if (board[row][col] == 0 || board[row - 1][col] == 0 || board[row][col] == board[row - 1][col]) {
                dead = false;
                break;
            }
        }
        if (!dead) break;
    }
    if (dead) GameState = UI_DEAD;
}