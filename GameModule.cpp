#include "GameModule.h"

void GameModule::initGameModules() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    pWin = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winSize.cx, winSize.cy, 0);
    pRenderer = SDL_CreateRenderer(pWin, -1, SDL_RENDERER_ACCELERATED);
}

void GameModule::uninitGameModules() {
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWin);
    pRenderer = nullptr;
    pWin = nullptr;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

/**
 * @name GameModule::runGame()
 * @brief 游戏的主体循环函数
 * @details
 *  通过SDL的计时器来记录游戏时间并按帧率更新游戏
 *  在游戏循环内通过调用`gameMechanic`的三个接口函数来实现游戏
 *      游戏键鼠交互 `gameMechanic->processGameEvent(evt);`
 *      游戏界面渲染 `gameMechanic->renderGame();`
 *      游戏状态更新 `gameMechanic->updateGame(frameMS);`
 */
void GameModule::runGame() const {
    int quit = 0;
    SDL_Event evt;

    Uint64 nFrequency, nPrevCounter, nCurrCounter, nElapseCounter;
    float elapseMS = 0.0f, lagMS = 0.0f, frameMS = 1000.0f / FPS;

    // SDL的计时器频率
    nFrequency = SDL_GetPerformanceFrequency();
    nPrevCounter = SDL_GetPerformanceCounter();

    while (!quit) {
        // 处理消息事件
        if (SDL_PollEvent(&evt))
            if (evt.type == SDL_QUIT)
                quit = 1;  // 窗口关闭，退出循环
            else
                gameMechanic->processGameEvent(evt);  // gameMechanic游戏键鼠交互
        else {
            nCurrCounter = SDL_GetPerformanceCounter();
            nElapseCounter = nCurrCounter - nPrevCounter;
            nPrevCounter = nCurrCounter;

            elapseMS = (nElapseCounter * 1000.0f) / nFrequency;  // 计算间隔时间
            lagMS += elapseMS;                                   // 累加间隔时长

            // 设置背景色
            SDL_SetRenderDrawColor(pRenderer, 250, 248, 239, 255);
            SDL_RenderClear(pRenderer);    // 清屏
            gameMechanic->renderGame();    // gameMechanic游戏界面渲染
            SDL_RenderPresent(pRenderer);  // 刷新渲染屏幕

            while (lagMS >= frameMS) {
                gameMechanic->updateGame(frameMS);  // gameMechanic游戏状态更新
                lagMS -= frameMS;                   // 重置累计间隔时间
            }
        }

        // 限制轮询的最高帧率，防止CPU占用过高
        Sleep(10);
    }
}