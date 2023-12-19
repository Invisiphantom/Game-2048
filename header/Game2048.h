#pragma once

#include <Windows.h>

#include <map>
#include <random>

#include "GameMechanic.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

// 方格底盘的位置和大小
static const int boardMargin_y_up = 150;   // 距窗口顶部的距离
static const int boardMargin_y_down = 60;  // 距窗口底部的距离
static const int boardMargin_x = 50;       // 距窗口左右两侧的距离
static const int boardSize = 550;          // 方格底盘的宽高大小

// 窗口大小和游戏帧率
static const LONG WINDOW_WIDTH = boardMargin_x * 2 + boardSize;
static const LONG WINDOW_HEIGHT = boardMargin_y_up + boardSize + boardMargin_y_down;
static const unsigned int FPS = 10u;  // 每秒刷新10帧

/**
 * @class Game2048
 * @brief 继承自GameMechanic，实现了2048游戏所需的三个接口
 * @details
 *  通过GameModule类的embedGameMechanic()被嵌入到游戏模块类中
 *  实现了抽象基类GameMechanic类的三个纯虚函数
 *  服务于GameModule::runGame()的游戏主体循环实现
 */
class Game2048 : public GameMechanic {
    // 定义七种游戏状态，分别是：准备动画、启动动画、游戏初始化、游戏运行、游戏胜利、游戏失败、无尽模式
    enum UIState { UI_READY = 0, UI_GO, UI_INIT, UI_RUN, UI_WIN, UI_DEAD, UI_INFINITY };

   private:
    SDL_Window* pWin = nullptr;         // 窗口指针
    SDL_Renderer* pRenderer = nullptr;  // 渲染器指针

    TTF_Font* titleFont = nullptr;      // 标题字体
    TTF_Font* scoreFont = nullptr;      // 分数字体
    TTF_Font* textFont = nullptr;       // 正文字体
    SDL_Texture* tScore = nullptr;      // 计分板贴图
    SDL_Texture* tBoard = nullptr;      // 方格底盘贴图
    std::map<int, SDL_Texture*> tNums;  // 方格数字贴图

    std::mt19937 randGen;          // 随机数生成器
    UIState GameState = UI_READY;  // 当前游戏状态
    int board[4][4] = {0};         // 方格数据
    int bestScore = 0;             // 最高分
    int score = 0;                 // 当前分数

   private:
    /* Game2048-Logic.cpp */
    void putNumber();  // 随机放置新数字
    void initBoard();  // 初始化方格
    void moveUp();     // 向上移动
    void moveDown();   // 向下移动
    void moveLeft();   // 向左移动
    void moveRight();  // 向右移动
    void checkWin();   // 检查是否胜利
    void checkDead();  // 检查是否失败

   public:
    explicit Game2048(SDL_Window* _pWin, SDL_Renderer* _pRenderer)
        : pWin(_pWin), pRenderer(_pRenderer), GameState(UI_READY) {
        randGen.seed(std::random_device()());  // 初始化随机数生成器
    }
    ~Game2048() {}

    /* Game2048-Render.cpp */
    void loadResources();    // 加载硬盘中的游戏字体、贴图等资源
    void unloadResources();  // 释放游戏资源

    /* Game2048-Interface.cpp */
    void processGameEvent(const SDL_Event evt);  // 处理键鼠事件
    void updateGame(const float ms);             // 更新游戏时间
    void renderGame() const;                     // 渲染游戏界面
};
