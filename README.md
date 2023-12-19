# SDL2+2048小游戏开发

SDL2游戏开发教程：[SDL2 游戏开发最小知识栈 - 没事造轮子](https://meishizaolunzi.com/sdl2-you-xi-kai-fa-zui-xiao-zhi-shi-zhan/)  
项目环境配置： [SDL2+2048小游戏开发-环境配置 | Mind City](https://invisiphantom.github.io/Skill/SDL2+2048%E5%B0%8F%E6%B8%B8%E6%88%8F%E5%BC%80%E5%8F%91-%E7%8E%AF%E5%A2%83%E9%85%8D%E7%BD%AE/)  
项目代码详解：[SDL2+2048小游戏开发-代码详解 | Mind City](https://invisiphantom.github.io/Skill/SDL2+2048%E5%B0%8F%E6%B8%B8%E6%88%8F%E5%BC%80%E5%8F%91-%E4%BB%A3%E7%A0%81%E8%AF%A6%E8%A7%A3/)  
项目源代码：[Invisiphantom/2048-Table](https://github.com/Invisiphantom/2048-Table)  
  
- 相关项目：[Invisiphantom/Game-Tetris](https://github.com/Invisiphantom/Game-Tetris)  

------
  
#### class GameModule  
游戏的最顶层模块是`GameModule`游戏模块类  
负责初始化SDL模块、执行游戏循环和释放SDL模块
  
```cpp
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
```

  
#### GameModule::runGame()
`GameModule::runGame()`是游戏的主体循环部分  
通过SDL的计时器来记录游戏时间并按帧率更新游戏  
  
在游戏循环内通过调用`gameMechanic`的三个接口函数来实现游戏  
- 游戏键鼠交互 `gameMechanic->processGameEvent(evt);`  
- 游戏界面渲染 `gameMechanic->renderGame();`  
- 游戏状态更新 `gameMechanic->updateGame(frameMS);`  
  
```cpp
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
```
  
#### class GameMechanic  
游戏的中间层是抽象基类`GameMechanic`  
它定义了规范派生类需要实现的那三个接口函数  
`processGameEvent(evt)`、`updateGame(frameMS)`和`renderGame()`  

```cpp
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
```

  
#### Game2048  
游戏的最底层是游戏的实现类`Game2048`  
它继承自GameMechanic，实现了2048游戏所需的三个接口  
通过GameModule类的embedGameMechanic()被嵌入到游戏模块类中  
服务于GameModule::runGame()的游戏主体循环实现  
  
##### 主要内部变量：
- 游戏状态的七个阶段
- SDL的渲染组件
	- SDL窗口指针
	- SDL渲染器指针
	- SDL字体指针
	- SDL贴图指针
- 游戏的机制组件
	- 随机数生成器
	- 当前游戏状态
	- 方格内数字
	- 历史最高分
	- 当前分数

##### 主要成员函数：
- 游戏的机制函数
	- 随机放置新数字
    - 初始化方格
    - 向上移动
    - 向下移动
    - 向左移动
    - 向右移动
    - 检查是否胜利
    - 检查是否失败
- 游戏的资源管理函数
    - 加载硬盘中的字体、贴图等资源
    - 释放游戏资源
- 游戏的接口函数
    - 处理键鼠事件
    - 更新游戏时间
    - 渲染游戏界面
  
```cpp
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
```
  

#### Summary  
通过GameModule、GameMechanic和Game2048三个类的组合，实现了一个简单的2048游戏  
并且此架构可以很方便地扩展到其他游戏上，只需要实现GameMechanic的三个接口函数即可

