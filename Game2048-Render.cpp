#include "Game2048.h"

/* 加载游戏资源 */
void Game2048::loadResources() {
    // 加载字体资源
    titleFont = TTF_OpenFont("font/Arial_Bold.ttf", 85);
    scoreFont = TTF_OpenFont("font/Arial_Bold.ttf", 20);
    textFont = TTF_OpenFont("font/HeiTi.ttf", 16);

    // 加载窗口小图标
    SDL_Surface* iconSurface = IMG_Load("img/favicon.ico");
    SDL_SetWindowIcon(pWin, iconSurface);
    SDL_FreeSurface(iconSurface);

    // 加载计分板贴图，方格底盘贴图和方格数字贴图
    tScore = IMG_LoadTexture(pRenderer, "img/ScoreBoard.png");
    tBoard = IMG_LoadTexture(pRenderer, "img/Board.png");
    for (int i = 2; i <= 32768; i *= 2) {
        char path[32];
        sprintf_s(path, "img/Num_%d.png", i);
        tNums[i] = IMG_LoadTexture(pRenderer, path);
    }
}

/* 释放游戏资源 */
void Game2048::unloadResources() {
    // 释放字体资源
    TTF_CloseFont(titleFont);
    TTF_CloseFont(scoreFont);
    TTF_CloseFont(textFont);

    // 释放贴图资源
    SDL_DestroyTexture(tScore);
    SDL_DestroyTexture(tBoard);
    for (auto& i : tNums) {
        SDL_DestroyTexture(i.second);
    }
}