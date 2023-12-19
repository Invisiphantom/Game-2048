#include "Game2048.h"

void Game2048::processGameEvent(const SDL_Event _evt) {
    if (_evt.type == SDL_KEYDOWN) {
        // 游戏准备时
        if (GameState == UI_READY) {
            // 按空格键开始游戏
            if (_evt.key.keysym.sym == SDLK_SPACE) {
                GameState = UI_GO;
            }
        }
        // 游戏运行时
        else if (GameState == UI_RUN || GameState == UI_INFINITY) {
            // 使用上下左右方向键控制游戏
            switch (_evt.key.keysym.sym) {
                case SDLK_UP:
                    moveUp();
                    break;
                case SDLK_DOWN:
                    moveDown();
                    break;
                case SDLK_LEFT:
                    moveLeft();
                    break;
                case SDLK_RIGHT:
                    moveRight();
                    break;
                default:
                    break;
            }
            // 按ESC键返回到准备状态
            if (_evt.key.keysym.sym == SDLK_ESCAPE) {
                GameState = UI_READY;
            }
        }
        // 游戏结束时
        else if (GameState == UI_WIN) {
            // 按空格键开始无尽模式
            if (_evt.key.keysym.sym == SDLK_SPACE) {
                GameState = UI_INFINITY;
            }
        } else if (GameState == UI_DEAD) {
            // 按空格键开始新游戏
            if (_evt.key.keysym.sym == SDLK_SPACE) {
                GameState = UI_INIT;
            }
        }
    }
}

void Game2048::updateGame(const float ms) {
    if (GameState == UI_GO) {
        // 休眠400ms，等待Go动画播放完毕
        Sleep(400);
        GameState = UI_INIT;
    } else if (GameState == UI_INIT) {
        initBoard();
        GameState = UI_RUN;
    }
}

// 标题字体位置
static const int titleMargin_y = 20;  // 距窗口顶部的距离
static const int titleMargin_x = 50;  // 距窗口左右的距离

// 计分板位置
static const int scoreSize_w = 230;   // 计分板宽度
static const int scoreSize_h = 65;    // 计分板高度
static const int scoreMargin_y = 40;  // 距窗口顶部的距离
static const int scoreMargin_x = WINDOW_WIDTH - boardMargin_x - scoreSize_w;

// 数字方格贴图位置
static const int cellMargin = 14;  // 数字方格之间的间距
static const int cellSize = 120;   // 数字方格的宽高

void Game2048::renderGame() const {
    // 渲染标题字体2048
    SDL_Color color = {119, 110, 101, 255};
    SDL_Surface* textSurface = TTF_RenderUTF8_Blended(titleFont, "2048", color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
    SDL_Rect dstRect = {titleMargin_x, titleMargin_y, textSurface->w, textSurface->h};
    SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

    // 渲染计分板贴图
    dstRect = {scoreMargin_x, scoreMargin_y, scoreSize_w, scoreSize_h};
    SDL_RenderCopy(pRenderer, tScore, nullptr, &dstRect);

    // 渲染分数字体
    char scoreText[32];
    static const int scoreBias = 51;  // 微调位置使字体居中
    sprintf_s(scoreText, "%d", score);
    color = {255, 255, 255, 255};
    textSurface = TTF_RenderUTF8_Blended(scoreFont, scoreText, color);
    textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
    dstRect = {scoreMargin_x + scoreBias - textSurface->w / 2, scoreMargin_y + 33, textSurface->w, textSurface->h};
    SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

    // 渲染最高分数字体
    static const int bestScoreBias = 165;  // 微调位置使字体居中
    sprintf_s(scoreText, "%d", bestScore);
    color = {255, 255, 255, 255};
    textSurface = TTF_RenderUTF8_Blended(scoreFont, scoreText, color);
    textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
    dstRect = {scoreMargin_x + bestScoreBias - textSurface->w / 2, scoreMargin_y + 33, textSurface->w, textSurface->h};
    SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

    // 渲染方格底盘贴图
    dstRect = {boardMargin_x, boardMargin_y_up, boardSize, boardSize};
    SDL_RenderCopy(pRenderer, tBoard, nullptr, &dstRect);

    // 渲染数字贴图
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            if (board[row][col] == 0) continue;
            dstRect.x = boardMargin_x + cellMargin + col * (cellMargin + cellSize);
            dstRect.y = boardMargin_y_up + cellMargin + row * (cellMargin + cellSize);
            dstRect.w = cellSize;
            dstRect.h = cellSize;
            SDL_RenderCopy(pRenderer, tNums.at(board[row][col]), nullptr, &dstRect);
        }
    }

    if (GameState == UI_READY) {
        // 设置底盘背景透明
        SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 96);
        SDL_Rect rect = {boardMargin_x, boardMargin_y_up, boardSize, boardSize};
        SDL_RenderFillRect(pRenderer, &rect);
        // 渲染Ready字体
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(titleFont, "Ready?", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        // 设置字体位于方格中心
        dstRect = {boardMargin_x + boardSize / 2 - textSurface->w / 2,
                   boardMargin_y_up + boardSize / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

        // 渲染底部操作指南
        static const int spaceBias = 10;  // 文字距离方格底盘下方的距离
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(textFont, "按 空格键 开始游戏，按 ESC 返回主菜单", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        dstRect = {boardMargin_x, WINDOW_HEIGHT - boardMargin_y_down + spaceBias, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

        static const int direcBias = 30;  // 文字距离方格底盘下方的距离
        textSurface = TTF_RenderUTF8_Blended(textFont, "用你的 方向键 移动方块, 数字到达2048即为胜利~", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        dstRect = {boardMargin_x, WINDOW_HEIGHT - boardMargin_y_down + direcBias, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);
    } else if (GameState == UI_GO) {
        // 设置底盘背景透明
        SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 96);
        SDL_Rect rect = {boardMargin_x, boardMargin_y_up, boardSize, boardSize};
        SDL_RenderFillRect(pRenderer, &rect);
        // 渲染Go字体
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(titleFont, "Go!", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        // 设置字体位于方格中心
        dstRect = {boardMargin_x + boardSize / 2 - textSurface->w / 2,
                   boardMargin_y_up + boardSize / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);
    } else if (GameState == UI_WIN) {
        // 设置底盘背景透明
        SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 96);
        SDL_Rect rect = {boardMargin_x, boardMargin_y_up, boardSize, boardSize};
        SDL_RenderFillRect(pRenderer, &rect);
        // 渲染胜利字体
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(titleFont, "Winner!", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        // 设置字体位于方格中心
        dstRect = {boardMargin_x + boardSize / 2 - textSurface->w / 2,
                   boardMargin_y_up + boardSize / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

        // 渲染底部操作指南
        static const int spaceBias = 10;
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(textFont, "你也太强了，按下 空格键 冲刺无尽模式！", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        dstRect = {boardMargin_x, WINDOW_HEIGHT - boardMargin_y_down + spaceBias, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);
    } else if (GameState == UI_DEAD) {
        // 设置底盘背景透明
        SDL_SetRenderDrawBlendMode(pRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 96);
        SDL_Rect rect = {boardMargin_x, boardMargin_y_up, boardSize, boardSize};
        SDL_RenderFillRect(pRenderer, &rect);
        // 渲染失败字体
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(titleFont, "Game over!", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        // 设置字体位于方格中心
        dstRect = {boardMargin_x + boardSize / 2 - textSurface->w / 2,
                   boardMargin_y_up + boardSize / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);

        // 渲染底部操作指南
        static const int spaceBias = 10;
        color = {119, 110, 101, 255};
        textSurface = TTF_RenderUTF8_Blended(textFont, "你也太菜了吧，按下 空格键 开始新游戏！", color);
        textTexture = SDL_CreateTextureFromSurface(pRenderer, textSurface);
        dstRect = {boardMargin_x, WINDOW_HEIGHT - boardMargin_y_down + spaceBias, textSurface->w, textSurface->h};
        SDL_RenderCopy(pRenderer, textTexture, nullptr, &dstRect);
    }
}