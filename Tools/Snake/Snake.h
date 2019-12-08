#pragma once

// 方向
#define TOUP 1
#define TODOWN 2
#define TOLEFT 3
#define TORIGHT 4

// SnakeGo 结果
#define GOFULLSCREEN 2
#define GOSUCCESS 1
#define GOGAMEOVER 0

// 棋盘属性
#define WID 18
#define HEI 10
#define FOOD -1
#define EMPTY 0

// 游戏选项
#define FAST 100
#define SLOW 200
#define DEFAULTLEN 4
#define FOODSIZE -3
#define TEXTSIZE 64

// 色彩
#define LINECOLOR RGB(138,187,251)
#define SNAKECOLOR RGB(87,104,152)
#define BUMPCOLOR RGB(255,72,0)
#define FOODCOLOR RGB(61,89,171)
#define BACKGROUNDCOLOR RGB(205,220,255)
#define FONTCOLOR RGB(87,104,152)

// 字符串
#define TEXTFONT  L"微软雅黑"
#define YOUWIN    L"游戏胜利"
#define GAMEOVER  L"游戏结束 "
#define GAMEPAUSE L"游戏暂停"
#define IMGERROR  L"调用图像资源失败"
#define POINTSTR  L"分数：%d"

#include <thread>
#include <chrono>

class CSnakeView
{
    HINSTANCE hInst;
    Window w;

    class Timer
    {
        bool canceled;
        std::thread timer;
    public:
        Timer() : canceled(false) {};
        void begin(std::chrono::milliseconds interval, std::function<void()> callback)
        {
            if (!canceled)
            {
                end();
                canceled = false;
            }
            timer = std::thread([](const Timer* timer,
                std::chrono::milliseconds interval,
                std::function<void()> callback)
            {
                while (!timer->canceled) {
                    std::this_thread::sleep_for(interval);
                    callback();
                }
            }, this, interval, callback);
        }
        void end()
        {
            canceled = true;
            timer.join();
        }
        ~Timer()
        {
            if (!canceled)
                end();
        }
    };

public:
    typedef std::pair<int, int> Size;
	int HeadPosX;				// 头的位置
	int HeadPosY;
	int FoodPosX;				// 食物位置
	int FoodPosY;
	int Board[WID + 1][HEI + 1];// 模拟棋盘
	int Direction;				// 运动方向
	int Mode;					// 游戏模式
	int EdgeWidth;				// 边宽，不计线宽
	int SnakeLen;				// 蛇的长度

	int Speed;					// 蛇的速度（定时器Interval属性）

	bool Eaten;					// 是否吃了东西
	bool isPause;				// 是否已经暂停
	bool StartNew;				// 是否需要开新游戏
	bool Using;					// 防止过快改变方向产生Bug

	// 重新初始化游戏
	void GameStart();
	// 结束游戏
	void GameOver();
	// 计算EdgeWidth
    void CountEdgeWidth(const Size& size);
	// 随机放置食物
	void SetFood();
	// 绘制食物
	void DrawFood(ImageMatrix& img);
	// 绘制棋盘
	void DrawBoard(ImageMatrix& img, Size& size);
	// 根据Board进行蛇的绘制
	void DrawSnake(ImageMatrix& img);
	// 根据Board及Direction进行蛇运动的计算
	int SnakeGo();
	// 在标题栏显示分数
	void ShowPointOnTitle();
	// 计算游戏得分
	int GamePoint();
    // 设置定时器
    void setTimer(int ms, std::function<void()> cb)
    {
        Timer timer(std::chrono::milliseconds(ms), cb);
        std::this_thread::sleep_for(10000ms);
    }

    CSnakeView();
    ~CSnakeView();

    void CreateGame();
	void OnDraw(const GdiCanvas&);
	void OnKeyDown(int key);
	void OnStart();
	void OnFastspeed();
	void OnSlowspeed();
};

// 绘制向上的头
inline void DrawHeadToUp(ImageMatrix& img, int EdgeWidth, int x, int y) {
    PlatformIndependenceCanvas::rectangle(
        img, 
        EdgeWidth * (x - 1) + x + EdgeWidth / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth / 4,
        EdgeWidth * (x - 1) + x + EdgeWidth * 3 / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth
    );
}

// 绘制向下的头
inline void DrawHeadToDown(ImageMatrix& img, int EdgeWidth, int x, int y) {
    PlatformIndependenceCanvas::rectangle(
        img,
        EdgeWidth * (x - 1) + x + EdgeWidth / 4,
        EdgeWidth * (y - 1) + y - 1,
        EdgeWidth * (x - 1) + x + EdgeWidth * 3 / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth * 3 / 4
    );
}

// 绘制向左的头
inline void DrawHeadToLeft(ImageMatrix& img, int EdgeWidth, int x, int y) {
    PlatformIndependenceCanvas::rectangle(
        img,
        EdgeWidth * (x - 1) + x + EdgeWidth / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth / 4,
        EdgeWidth * (x - 1) + x + EdgeWidth,
        EdgeWidth * (y - 1) + y + EdgeWidth * 3 / 4
    );
}

// 绘制向右的头
inline void DrawHeadToRight(ImageMatrix& img, int EdgeWidth, int x, int y) {
    PlatformIndependenceCanvas::rectangle(
        img,
        EdgeWidth * (x - 1) + x - 1,
        EdgeWidth * (y - 1) + y + EdgeWidth / 4,
        EdgeWidth * (x - 1) + x + EdgeWidth * 3 / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth * 3 / 4
    );
}
