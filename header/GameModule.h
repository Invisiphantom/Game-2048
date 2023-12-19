#pragma once

#include <Windows.h>

#include "GameMechanic.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

/**
 * @class GameModule
 * @brief 游戏模块类，负责初始化SDL模块、执行游戏循环和释放SDL模块
 */
class GameModule {
   private:
    SDL_Window* pWin = nullptr;         // 窗口指针
    SDL_Renderer* pRenderer = nullptr;  // 渲染器指针

    const char* title;        // 窗口标题
    unsigned int FPS = 0u;    // 游戏帧率
    SIZE winSize = {0l, 0l};  // 窗口大小

    /* 游戏机制GameMechanic类，通过embedGameMechanic()嵌入 */
    /* runGame()就是通过GameMechanic的三个接口来实现游戏循环的 */
    GameMechanic* gameMechanic = nullptr;

   public:
    explicit GameModule(const char* _title, LONG _winSizeWidth, LONG _winSizeHeight, unsigned int _FPS)
        : title(_title), winSize({_winSizeWidth, _winSizeHeight}), FPS(_FPS) {}
    ~GameModule() {}

    void initGameModules();    // 初始化SDL模块
    void uninitGameModules();  // 释放SDL模块
    void runGame() const;      // 开始执行游戏循环

    // 暴露SDL的窗口和渲染器指针，用于初始化GameMechanic
    SDL_Window* getpWin() { return pWin; }
    SDL_Renderer* getpRenderer() { return pRenderer; }

    // 嵌入游戏机制
    void embedGameMechanic(GameMechanic& _GameMechanic) { gameMechanic = &_GameMechanic; }
};