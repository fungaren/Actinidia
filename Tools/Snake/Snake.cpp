/*
 * Copyright (c) 2020, FANG All rights reserved.
 */
#include <windows.h>
#undef max

#include "resource.h"

#include "Common/Window.h"
#include "Common/Canvas.h"
#include "Common/Timer.h"
#include "Snake.h"

SnakeView::SnakeView(HINSTANCE i, HWND parent) : 
    hInst(i), Direction(TORIGHT), Eaten(false), EdgeWidth(0),
    FoodPosX(0), FoodPosY(0), HeadPosX(0), HeadPosY(0), SnakeLen(0), Using(false)
{
    w.setElseHandler([](uint32_t message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_GETMINMAXINFO:
            MINMAXINFO* lpMMI;  // Minimum size
            lpMMI = (MINMAXINFO*)lParam;
            lpMMI->ptMinTrackSize.x = 600;
            lpMMI->ptMinTrackSize.y = 360;
            return true;
        }
        return false;
    });
    w.setPaintHandler([this](const GdiCanvas& gdi) {
        this->OnDraw(gdi);
    });
    w.setKeyDownHandler([this](int key) {
        this->OnKeyDown(key);
    });
    w.create(L"Snake", 600, 400, 600, 400, parent);

    GameState = State::Welcome;
}

void SnakeView::OnDraw(const GdiCanvas& gdi)
{
    int width, height;
    auto& size = w.getSize();
    std::tie(width, height) = size;
    if (GameState == State::Welcome)
    {
        auto temp = ImageMatrixFactory::createBufferImage(width, height, BACKGROUNDCOLOR);
        auto welcome = ImageMatrixFactory::fromPngResource(IDB_WELCOME, L"PNG", hInst);
        PiCanvas::blend(temp, welcome, (width - welcome->getWidth()) / 2, (height - welcome->getHeight()) / 2,
            welcome->getWidth(), welcome->getHeight(), 0, 0, welcome->getWidth(), welcome->getHeight(), 255);
        gdi.paste(temp, 0, 0);
        return;
    }

    // 游戏已经开始，重绘一般由定时器触发，也可能通过窗口缩放触发
    CountEdgeWidth(size);    // 重新计算边宽

    // 清空屏幕 设置背景颜色
    auto img = ImageMatrixFactory::createBufferImage(width, height, BACKGROUNDCOLOR);

    DrawBoard(img, size);    // 绘制棋盘
    DrawSnake(img);          // 绘制蛇
    DrawFood(img);           // 绘制食物

    switch (GameState)
    {
    case State::Gaming:
        gdi.paste(img, 0, 0);
        break;
    case State::GameOver:
    {
        StopTimer();
        // 产生碰撞效果
        switch (Direction)
        {
        case TOUP:
            DrawHeadToDown(img, EdgeWidth, HeadPosX, HeadPosY, BUMPCOLOR);
            break;
        case TODOWN:
            DrawHeadToUp(img, EdgeWidth, HeadPosX, HeadPosY, BUMPCOLOR);
            break;
        case TOLEFT:
            DrawHeadToRight(img, EdgeWidth, HeadPosX, HeadPosY, BUMPCOLOR);
            break;
        case TORIGHT:
            DrawHeadToLeft(img, EdgeWidth, HeadPosX, HeadPosY, BUMPCOLOR);
            break;
        default:
            break;
        }
        gdi.paste(img, 0, 0);
        // 结束游戏
        stringstream_t buf;
        buf << GAMEOVER << ' ' << SCORE << GamePoint();
        gdi.printText(150, 100, buf.str(), (uint16_t)buf.str().size(), TEXTFONT, TEXTSIZE, FONTCOLOR);
        break;
    }
    case State::Pause:
        gdi.paste(img, 0, 0);
        // 显示暂停文字
        gdi.printText(150, 100, GAMEPAUSE, lstrlen(GAMEPAUSE), TEXTFONT, TEXTSIZE, FONTCOLOR);
        break;
    }
}

// 计算EdgeWidth
void SnakeView::CountEdgeWidth(const std::pair<int, int>& size)
{
    if (size.first * HEI / WID <= size.second)
    {
        // 宽度小高度大
        EdgeWidth = size.first / WID - 1;
    }
    else
    {
        // 宽度大高度小
        EdgeWidth = size.second / HEI - 1;
    }
}

// 随机放置一个食物
void SnakeView::SetFood()
{
    int a, b;                           // 记录坐标
    srand((unsigned)time(NULL));        // 随机种子
    if (SnakeLen < WID * HEI / 2) {     // 当蛇长很小时
        do {
            a = rand() % WID + 1;       // 生成从1到WID的随机数
            b = rand() % HEI + 1;       // 生成从1到HEI的随机数
        } while (Board[a][b] != EMPTY);
    }
    else       // 当蛇很长时
    {
        int R[WID * HEI / 2 + 1][2];    // 保存空位的坐标
        int t = 0;
        for (int i = 1; i <= WID; i++) {
            for (int j = 1; j <= HEI; j++) {
                if (Board[i][j] == EMPTY) { // 寻找所有空位
                    R[t][0] = i;
                    R[t++][1] = j;
                }
            }
        }
        int r = rand() % t;             // 生成0到t-1的随机数
        a = R[r][0];
        b = R[r][1];                    // 得到空位坐标
    }
    Board[a][b] = FOOD;                 // 设置食物
    FoodPosX = a;
    FoodPosY = b;
    ShowPointOnTitle();                 // 显示分数
}

// 绘制食物
void SnakeView::DrawFood(pImageMatrix img)
{
    pImageMatrix food;
    try {
        food = ImageMatrixFactory::fromPngResource(IDB_FOOD, L"PNG", hInst);
    }
    catch (ustr_error& e) {
        w.alert(string_t(ustr("Failed to load image: ")) + e.what(), L"Error");
        return;
    }
    PiCanvas::blend(img,
        food,
        EdgeWidth * (FoodPosX - 1) + FoodPosX - FOODSIZE,
        EdgeWidth * (FoodPosY - 1) + FoodPosY - FOODSIZE,
        EdgeWidth + 2 * (FOODSIZE),
        EdgeWidth + 2 * (FOODSIZE),
        0, 0, food->getWidth(), food->getHeight(), 255
    );
}

// 绘制棋盘
void SnakeView::DrawBoard(pImageMatrix img, Size& size)
{
    HPEN pen = CreatePen(PS_SOLID, 1, LINECOLOR);   // 边框

    for (int i = 0; i < WID; i++)           // 横向格子
        PiCanvas::rectangle(img,
            EdgeWidth * i + i,
            0,
            EdgeWidth * (i + 1) + i + 2,
            EdgeWidth * HEI + HEI + 1,
            Canvas::solid,
            LINECOLOR
        );
    for (int i = 0; i < HEI; i++)           // 纵向格子
        PiCanvas::rectangle(img,
            0,
            EdgeWidth * i + i,
            EdgeWidth * WID + WID + 1,
            EdgeWidth*(i + 1) + i + 2,
            Canvas::solid,
            LINECOLOR
        );
}

// 在标题栏显示分数
void SnakeView::ShowPointOnTitle()
{
    stringstream_t buf;
    buf << SCORE << GamePoint();
    w.setTitle(buf.str());
}

// 计算游戏得分
int SnakeView::GamePoint()
{
    return (SnakeLen - DEFAULTLEN) * 100;   // 每吃一个食物得100分
}

// 根据Board进行蛇的绘制
void SnakeView::DrawSnake(pImageMatrix img)
{
    int t = 1;              // 计数
    int x = HeadPosX;       // 当前位置的坐标值
    int y = HeadPosY;
    int leftx, lefty;       // 左边的坐标值
    int rightx, righty;     // 右边的坐标值
    int upx, upy;           // 上边的坐标值
    int downx, downy;       // 下边的坐标值
    while (t <= SnakeLen) {

        // 边缘处理x
        switch (x)
        {
        case 1:             // 屏幕最左边
            leftx = WID;
            rightx = x + 1;
            break;
        case WID:           // 屏幕最右边
            leftx = x - 1;
            rightx = 1;
            break;
        default:            // 屏幕中间
            leftx = x - 1;
            rightx = x + 1;
            break;
        }
        upx = x;            // 上下边的x值不变
        downx = x;

        // 边缘处理y
        switch (y)
        {
        case 1:             // 屏幕最上边
            upy = HEI;
            downy = y + 1;
            break;
        case HEI:           // 屏幕最下边
            upy = y - 1;
            downy = 1;
            break;
        default:            // 屏幕中间
            upy = y - 1;
            downy = y + 1;
            break;
        }
        lefty = y;          // 左右边的y值不变
        righty = y;

        // 对上一截的位置进行判断
        if ((Board[leftx][lefty] == (t - 1)) && (t != 1))   // 左边是上一截
        {
            DrawHeadToRight(img, EdgeWidth, x, y, SNAKECOLOR); // 向右的头
        }
        if ((Board[rightx][righty] == (t - 1)) && (t != 1)) // 右边是上一截
        {
            DrawHeadToLeft(img, EdgeWidth, x, y, SNAKECOLOR);  // 向左的头
        }
        if ((Board[upx][upy] == (t - 1)) && (t != 1))       // 上边是上一截
        {
            DrawHeadToDown(img, EdgeWidth, x, y, SNAKECOLOR);  // 向下的头
        }
        if ((Board[downx][downy] == (t - 1)) && (t != 1))   // 下边是上一截
        {
            DrawHeadToUp(img, EdgeWidth, x, y, SNAKECOLOR);    // 向上的头
        }

        // 对下一截的位置进行判断
        if (Board[leftx][lefty] == (t + 1))                 // 左边是下一截
        {
            DrawHeadToRight(img, EdgeWidth, x, y, SNAKECOLOR); // 向右的头
            x = (x == 1) ? WID : x - 1;                     // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        if (Board[rightx][righty] == (t + 1))               // 右边是下一截
        {
            DrawHeadToLeft(img, EdgeWidth, x, y, SNAKECOLOR);  // 向左的头
            x = (x == WID) ? 1 : x + 1;                     // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        if (Board[upx][upy] == (t + 1))                     // 上边是下一截
        {
            DrawHeadToDown(img, EdgeWidth, x, y, SNAKECOLOR);  // 向下的头
            y = (y == 1) ? HEI : y - 1;                     // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        if (Board[downx][downy] == (t + 1))                 // 下边是下一截
        {
            DrawHeadToUp(img, EdgeWidth, x, y, SNAKECOLOR);    // 向上的头
            y = (y == HEI) ? 1 : y + 1;                     // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        // 上下左右都找不到下一截，说明此处是末尾
        break;
    }
}

// 根据Board及Direction进行蛇运动的计算
// 仅当需要输出彩蛋时返回False
int SnakeView::SnakeGo()
{
    // 分析
    int x = HeadPosX;       // 头部的坐标值
    int y = HeadPosY;
    int leftx, lefty;       // 左边的坐标值
    int rightx, righty;     // 右边的坐标值
    int upx, upy;           // 上边的坐标值
    int downx, downy;       // 下边的坐标值
    bool HeadEatTail = false;   // 解决首尾相撞

    // 处理头部

    // 边缘处理x
    switch (x)
    {
    case 1:                 // 屏幕最左边
        leftx = WID;
        rightx = x + 1;
        break;
    case WID:               // 屏幕最右边
        leftx = x - 1;
        rightx = 1;
        break;
    default:                // 屏幕中间
        leftx = x - 1;
        rightx = x + 1;
        break;
    }
    upx = x;                // 上下边的x值不变
    downx = x;
    // 边缘处理y
    switch (y)
    {
    case 1:                 // 屏幕最上边
        upy = HEI;
        downy = y + 1;
        break;
    case HEI:               // 屏幕最下边
        upy = y - 1;
        downy = 1;
        break;
    default:                // 屏幕中间
        upy = y - 1;
        downy = y + 1;
        break;
    }
    lefty = y;              // 左右边的y值不变
    righty = y;

    // 处理蛇头移动
    switch (Direction)
    {
    case TOLEFT:
        // 对左一格的物体进行判断
        switch (Board[leftx][lefty])
        {
        case EMPTY:                         // 空格子
            Board[leftx][lefty] = 1;        // 往左边方向设置一个蛇头
            break;
        case FOOD:                          // 食物
            Board[leftx][lefty] = 1;        // 往左边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)  // 如果满屏爆表了
            {
                GameState = State::GameOver;
                return GOFULLSCREEN;        // 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:                            // 碰到身体
            if (Board[leftx][lefty] != SnakeLen)
            {
                GameState = State::GameOver;
                return GOGAMEOVER;          // 返回
            }
            else
            {
                Board[leftx][lefty] = 1;    // 碰到尾巴不算，往左边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = leftx;                   // 更新蛇头位置
        HeadPosY = lefty;
        break;
    case TORIGHT:
        // 对右一格的物体进行判断
        switch (Board[rightx][righty])
        {
        case EMPTY:                         // 空格子
            Board[rightx][righty] = 1;      // 往右边方向设置一个蛇头
            break;
        case FOOD:                          // 食物
            Board[rightx][righty] = 1;      // 往右边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)  // 如果满屏爆表了
            {
                GameState = State::GameOver;
                return GOFULLSCREEN;        // 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:                            // 碰到身体
            if (Board[rightx][righty] != SnakeLen)
            {
                GameState = State::GameOver;
                return GOGAMEOVER;          // 返回
            }
            else
            {
                Board[rightx][righty] = 1;  // 碰到尾巴不算，往右边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = rightx;                  // 更新蛇头位置
        HeadPosY = righty;
        break;
    case TOUP:
        // 对上一格的物体进行判断
        switch (Board[upx][upy])            // 对该方向的下一格进行判断
        {
        case EMPTY:                         // 空格子
            Board[upx][upy] = 1;            // 往上边方向设置一个蛇头
            break;
        case FOOD:                          // 食物
            Board[upx][upy] = 1;            // 往上边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)  // 如果满屏爆表了
            {
                GameState = State::GameOver;
                return GOFULLSCREEN;        // 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:                            // 碰到身体
            if (Board[upx][upy] != SnakeLen)
            {
                GameState = State::GameOver;
                return GOGAMEOVER;          // 返回
                break;
            }
            else
            {
                Board[upx][upy] = 1;        // 碰到尾巴不算，往上边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = upx;                     // 更新蛇头位置
        HeadPosY = upy;
        break;
    case TODOWN:
        // 对下一格的物体进行判断
        switch (Board[downx][downy])        // 对该方向的下一格进行判断
        {
        case EMPTY:                         // 空格子
            Board[downx][downy] = 1;        // 往下边方向设置一个蛇头
            break;
        case FOOD:                          // 食物
            Board[downx][downy] = 1;        // 往下边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)  // 如果满屏爆表了
            {
                GameState = State::GameOver;
                return GOFULLSCREEN;        // 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:                            // 碰到身体
            if (Board[downx][downy] != SnakeLen)
            {
                GameState = State::GameOver;
                return GOGAMEOVER;          // 返回
            }
            else
            {
                Board[downx][downy] = 1;    // 碰到尾巴不算，往下边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = downx;                   // 更新蛇头位置
        HeadPosY = downy;
        break;
    default:
        return GOFULLSCREEN;                // 没有方向？这不科学。就当做输出彩蛋吧
    }

    // 其余部分的移动（x,y为原来蛇的第一截，新蛇的第二截）
    int t = 1;              // 原来蛇的第一截，新蛇的第二截

    while (t <= SnakeLen) {
        // 边缘处理x
        switch (x)
        {
        case 1:             // 屏幕最左边
            leftx = WID;
            rightx = x + 1;
            break;
        case WID:           // 屏幕最右边
            leftx = x - 1;
            rightx = 1;
            break;
        default:            // 屏幕中间
            leftx = x - 1;
            rightx = x + 1;
            break;
        }
        upx = x;            // 上下边的x值不变
        downx = x;

        // 边缘处理y
        switch (y)
        {
        case 1:             // 屏幕最上边
            upy = HEI;
            downy = y + 1;
            break;
        case HEI:           // 屏幕最下边
            upy = y - 1;
            downy = 1;
            break;
        default:            // 屏幕中间
            upy = y - 1;
            downy = y + 1;
            break;
        }
        lefty = y;          // 左右边的y值不变
        righty = y;

        // 对下一截的位置进行判断
        if (Board[leftx][lefty] == (t + 1))     // 左边是下一截
        {
            Board[x][y]++;                      // 蛇移动
            x = (x == 1) ? WID : x - 1;         // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        if (Board[rightx][righty] == (t + 1))   // 右边是下一截
        {
            Board[x][y]++;                      // 蛇移动
            x = (x == WID) ? 1 : x + 1;         // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        if (Board[upx][upy] == (t + 1))         // 上边是下一截
        {
            Board[x][y]++;                      // 蛇移动
            y = (y == 1) ? HEI : y - 1;         // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        if (Board[downx][downy] == (t + 1))     // 下边是下一截
        {
            Board[x][y]++;                      // 蛇移动
            y = (y == HEI) ? 1 : y + 1;         // 下一截蛇的身体
            t++;                                // 计数+1
            continue;                           // 跳出
        }
        // 如果能找到下一截，说明此处不是末尾，此时 x,y 都变成了刚才找到的下一截的位置
        // 上下左右都找不到下一截，说明此处是末尾，此时 x,y 仍然是当前位置
        if (Eaten) {
            Board[x][y] = t + 1;                // 尾巴延长
            SnakeLen++;                         // 蛇长+1
            Eaten = false;
            SetFood();                          // 再放置一个食物
        }
        else
        {
            if (HeadEatTail)
                Board[x][y] = t + 1;            // 首尾相接，把被头覆盖掉的尾巴补回
            else
                Board[x][y] = EMPTY;            // 清除末尾
        }
        break;
    }
    return GOSUCCESS;
}

void SnakeView::OnKeyDown(int key)
{
    switch (key)
    {
    case VK_RETURN:
    case VK_SPACE:
        switch (GameState)
        {
        case State::GameOver:
        case State::Welcome:
            memset(Board, 0, sizeof(Board));    // 初始化棋盘
            Using = false;                      // 默认未占用方向
            Direction = TORIGHT;                // 默认方向向右
            Eaten = false;                      // 没吃食物

            SnakeLen = DEFAULTLEN;              // 默认长度
            HeadPosX = (WID + DEFAULTLEN) / 2;  // 默认蛇的位置X
            HeadPosY = HEI / 2;                 // 默认蛇的位置Y
            for (int i = 0; i < SnakeLen; i++) {
                Board[HeadPosX - i][HeadPosY] = i + 1;  // 放置初始蛇
            }

            SetFood();         // 设置食物
            SetTimer(SPEED);   // 开启定时器
            GameState = State::Gaming;
            break;
        case State::Pause:
            SetTimer(SPEED);
            GameState = State::Gaming;  // 取消暂停
            break;
        case State::Gaming:
            StopTimer();
            GameState = State::Pause;  // 暂停游戏
            break;
        }
        break;
    case VK_UP:
        if (Direction != TODOWN && Using == false)
        {
            // 往下走不能突然向上,被占用时也不能更改方向
            Direction = TOUP;                   // 方向向上
            Using = true;                       // 开始占用
        }
        break;
    case VK_DOWN:
        if (Direction != TOUP && Using == false)
        {
            // 往上走不能突然向下,被占用时也不能更改方向
            Direction = TODOWN;                 // 方向向下
            Using = true;                       // 开始占用
        }
        break;
    case VK_LEFT:
        if (Direction != TORIGHT && Using == false)
        {
            // 往右走不能突然向左,被占用时也不能更改方向
            Direction = TOLEFT;                 // 方向向左
            Using = true;                       // 开始占用
        }
        break;
    case VK_RIGHT:
        if (Direction != TOLEFT && Using == false)
        {
            // 往左走不能突然向右,被占用时也不能更改方向
            Direction = TORIGHT;                // 方向向右
            Using = true;                       // 开始占用
        }
        break;
    }
}

void SnakeView::OnTimer()
{
    Using = false;                  // 解除占用
    // 定时器只负责调用蛇运动的计算，接着在画图消息里重绘
    switch (SnakeGo())  // 分析蛇 吃了食物自动SetFood
    {
    case GOSUCCESS:
        w.refresh();
        break;
    case GOGAMEOVER:
        w.refresh();
        break;
    case GOFULLSCREEN:
        w.refresh();
        w.alert(YOUWIN, YOUWIN); // 输出彩蛋
        break;
    }
}
