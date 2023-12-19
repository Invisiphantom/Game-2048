#include "Game2048.h"
#include "GameModule.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // 实例化GameModule类
    GameModule GM("Game-2048", WINDOW_WIDTH, WINDOW_HEIGHT, FPS);
    GM.initGameModules();  // 初始化SDL模块

    // 使用GM的窗口指针和渲染器指针实例化Game2048类
    Game2048 game2048(GM.getpWin(), GM.getpRenderer());
    game2048.loadResources();  // 加载游戏资源

    // 将Game2048类嵌入到GameModule类中
    GM.embedGameMechanic(game2048);
    GM.runGame();  // 开始执行游戏循环

    // 游戏循环结束，释放资源
    game2048.unloadResources();
    GM.uninitGameModules();
    return 0;
}