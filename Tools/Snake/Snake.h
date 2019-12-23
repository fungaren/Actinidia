﻿#pragma once

// 方向
#define TOUP     1
#define TODOWN   2
#define TOLEFT   3
#define TORIGHT  4

// SnakeGo 结果
#define GOFULLSCREEN 2
#define GOSUCCESS    1
#define GOGAMEOVER   0

// 棋盘属性
#define WID    18
#define HEI    11
#define FOOD    -1
#define EMPTY  0

// 游戏选项 蛇的速度（定时器Interval属性）
#define SPEED      100
#define DEFAULTLEN 4
#define FOODSIZE   -3
#define TEXTSIZE   64

// 色彩
#define LINECOLOR       Canvas::rgb(138,187,251)
#define SNAKECOLOR      Canvas::rgb(87,104,152)
#define BUMPCOLOR       Canvas::rgb(255,72,0)
#define FOODCOLOR       Canvas::rgb(61,89,171)
#define BACKGROUNDCOLOR Canvas::rgb(205,220,255)
#define FONTCOLOR       Canvas::rgb(87,104,152)

// 字符串
#define TEXTFONT  L"微软雅黑"
#define YOUWIN    L"游戏胜利"
#define GAMEOVER  L"游戏结束 "
#define GAMEPAUSE L"游戏暂停"
#define IMGERROR  L"调用图像资源失败"
#define SCORE     L"分数: "

class SnakeView
{
    HINSTANCE hInst;
    Window w;
    Timer timer;

    typedef std::pair<int, int> Size;
    int HeadPosX;               // 头的位置
    int HeadPosY;
    int FoodPosX;               // 食物位置
    int FoodPosY;
    int Board[WID + 1][HEI + 1];// 模拟棋盘
    int Direction;              // 运动方向
    int Mode;                   // 游戏模式
    int EdgeWidth;              // 边宽，不计线宽
    int SnakeLen;               // 蛇的长度
    bool Eaten;                 // 是否吃了东西
    
    enum State {
        Gaming, GameOver, Welcome, Pause
    };
    State GameState;
    bool Using;                 // 防止过快改变方向产生Bug

public:
    // Constructor
    SnakeView(HINSTANCE i, HWND parent);
    // 随机放置食物
    void SetFood();
    // 绘制食物
    void DrawFood(ImageMatrix& img);
    // 绘制棋盘
    void DrawBoard(ImageMatrix& img, Size& size);
    // 根据Board进行蛇的绘制
    void DrawSnake(ImageMatrix& img);
    // 计算边宽
    void CountEdgeWidth(const std::pair<int, int>& size);
    // 根据Board及Direction进行蛇运动的计算
    int SnakeGo();
    // 在标题栏显示分数
    void ShowPointOnTitle();
    // 计算游戏得分
    int GamePoint();
    // 设置定时器
    void SetTimer(int ms) {
        timer.begin(std::chrono::milliseconds(ms), std::bind(&SnakeView::OnTimer, this));
    }
    void StopTimer() {
        timer.end();
    }

    // Message Handlers
    void OnDraw(const GdiCanvas&);
    void OnKeyDown(int key);
    void OnTimer();
};

// 绘制向上的头
inline void DrawHeadToUp(ImageMatrix& img, int EdgeWidth, int x, int y, Canvas::color color) {
    PlatformIndependenceCanvas::fillSolidRect(
        img, 
        EdgeWidth * (x - 1) + x + EdgeWidth / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth / 4,
        EdgeWidth * (x - 1) + x + EdgeWidth * 3 / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth,
        color
    );
}

// 绘制向下的头
inline void DrawHeadToDown(ImageMatrix& img, int EdgeWidth, int x, int y, Canvas::color color) {
    PlatformIndependenceCanvas::fillSolidRect(
        img,
        EdgeWidth * (x - 1) + x + EdgeWidth / 4,
        EdgeWidth * (y - 1) + y - 1,
        EdgeWidth * (x - 1) + x + EdgeWidth * 3 / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth * 3 / 4,
        color
    );
}

// 绘制向左的头
inline void DrawHeadToLeft(ImageMatrix& img, int EdgeWidth, int x, int y, Canvas::color color) {
    PlatformIndependenceCanvas::fillSolidRect(
        img,
        EdgeWidth * (x - 1) + x + EdgeWidth / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth / 4,
        EdgeWidth * (x - 1) + x + EdgeWidth,
        EdgeWidth * (y - 1) + y + EdgeWidth * 3 / 4,
        color
    );
}

// 绘制向右的头
inline void DrawHeadToRight(ImageMatrix& img, int EdgeWidth, int x, int y, Canvas::color color) {
    PlatformIndependenceCanvas::fillSolidRect(
        img,
        EdgeWidth * (x - 1) + x - 1,
        EdgeWidth * (y - 1) + y + EdgeWidth / 4,
        EdgeWidth * (x - 1) + x + EdgeWidth * 3 / 4,
        EdgeWidth * (y - 1) + y + EdgeWidth * 3 / 4,
        color
    );
}
