#pragma once
#include "SDL.h"

/**
 * @class GameMechanic
 * @brief 充当GameModule和底层游戏机制的层间接口
 * @details
 * 	定义了规范派生类需要实现的三个接口函数
 * 	这些接口函数在GameModule类的runGame()中被调用
 */
class GameMechanic {
   public:
    GameMechanic() {}
    virtual ~GameMechanic() {}

    /* 处理键鼠事件 */
    virtual void processGameEvent(const SDL_Event evt) = 0;

    /* 更新游戏时间 */
    virtual void updateGame(const float ms) = 0;

    /* 渲染游戏界面 */
    virtual void renderGame() const = 0;
};
