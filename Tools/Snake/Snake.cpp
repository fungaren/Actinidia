#include "pch.h"
#include "resource.h"
#include "Common/Window.h"
#include "Common/Canvas.h"
#include "Snake.h"

CSnakeView::CSnakeView()
{
    w.setElseHandler([](uint32_t message, uint32_t lParam, uint32_t wParam) {
        switch (message) {
        case WM_GETMINMAXINFO:
            MINMAXINFO* lpMMI;  // Minimum size
            lpMMI = (MINMAXINFO*)lParam;
            lpMMI->ptMinTrackSize.x = 600;
            lpMMI->ptMinTrackSize.y = 390;
        default:
            return false;
        }
        return true;
    });
    w.create(L"Snake", 600, 400);
    w.setPaintHandler(std::bind(&CSnakeView::OnDraw, this));
}

void CSnakeView::CreateGame()
{
    Speed = FAST;
    StartNew = true;
}

CSnakeView::~CSnakeView()
{

}

void CSnakeView::OnDraw(const GdiCanvas& gdi)
{
    auto& size = w.getSize();
    if (StartNew) {
        gdi.fillSolidRect(0, 0, size.first, size.second, BACKGROUNDCOLOR);

        ImageMatrix welcome = ImageMatrixFactory::fromPngResource(IDB_WELCOME, L"PNG", hInst);
        gdi.paste(welcome, (size.first - welcome.getWidth()) / 2, (size.second - welcome.getHeight()) / 2,
            welcome.getWidth(), welcome.getHeight(), 0, 0, welcome.getWidth(), welcome.getHeight());
        return;
    }
    // 游戏已经开始，重绘一般由定时器触发，也可能通过窗口缩放触发
    CountEdgeWidth(size);    // 重新计算边宽

    // 清空屏幕 设置背景颜色
    ImageMatrix img = ImageMatrixFactory::createBufferImage(size.first, size.second, BACKGROUNDCOLOR);

    DrawBoard(img, size);    // 绘制棋盘
    DrawSnake(img);          // 绘制蛇
    DrawFood(img);           // 绘制食物

    gdi.paste(img, 0, 0);
    if (isPause)
        gdi.printText(150, 100, GAMEPAUSE, 4, TEXTFONT, TEXTSIZE, FONTCOLOR); // 显示暂停文字
}

// 重新初始化游戏
void CSnakeView::GameStart()
{
    memset(Board, 0, sizeof(Board));	// 初始化棋盘
    isPause = false;					// 未暂停状态
    StartNew = false;
    Using = false;						// 默认未占用方向
    Direction = TORIGHT;				// 默认方向向右
    Eaten = false;						// 没吃食物

    SnakeLen = DEFAULTLEN;				// 默认长度
    HeadPosX = (WID + DEFAULTLEN) / 2;	// 默认蛇的位置X
    HeadPosY = HEI / 2;					// 默认蛇的位置Y
    for (int i = 0; i < SnakeLen; i++) {
        Board[HeadPosX - i][HeadPosY] = i + 1;	// 放置初始蛇
    }

    SetFood();		// 设置食物

    SetTimer(hwnd, 1, Speed, NULL);		// 开启定时器
}

// 结束游戏
void CSnakeView::GameOver()
{
    KillTimer(hwnd, 1);

    HDC pDC = GetDC(hwnd);

    // 产生碰撞效果

    HPEN pen = CreatePen(PS_SOLID, 1, SNAKECOLOR);
    HBRUSH br = CreateSolidBrush(BUMPCOLOR);
    HPEN pOldpen = (HPEN)SelectObject(pDC, pen);
    HBRUSH pOldbrush = (HBRUSH)SelectObject(pDC, br);
    switch (Direction)
    {
    case TOUP:
        DrawHeadToDown(pDC, EdgeWidth, HeadPosX, HeadPosY);
        break;
    case TODOWN:
        DrawHeadToUp(pDC, EdgeWidth, HeadPosX, HeadPosY);
        break;
    case TOLEFT:
        DrawHeadToRight(pDC, EdgeWidth, HeadPosX, HeadPosY);
        break;
    case TORIGHT:
        DrawHeadToLeft(pDC, EdgeWidth, HeadPosX, HeadPosY);
        break;
    default:
        break;
    }
    SelectObject(pDC, pOldpen);
    SelectObject(pDC, pOldbrush);
    DeleteObject(pOldpen);
    DeleteObject(pen);			// 删除资源
    DeleteObject(pOldbrush);
    DeleteObject(br);			// 删除资源

    // 结束游戏
    wchar_t buf[10];
    wnsprintf(buf, 9 * sizeof(wchar_t), POINTSTR, GamePoint());
    wchar_t buff[13];
    wnsprintf(buff, 12 * sizeof(wchar_t), "%s%7s", GAMEOVER, buf);

    gdi.printText(150, 100, buff, 12, TEXTFONT, TEXTSIZE, FONTCOLOR); // 显示暂停文字
    StartNew = true;			// 下次要按空格开始游戏

    RECT r;
    GetClientRect(hwnd, &r);
    ValidateRect(hwnd, r);
    ReleaseDC(hwnd, pDC);
}


// 计算EdgeWidth
void CSnakeView::CountEdgeWidth(const std::pair<int, int>& size)
{
    if (size.first * HEI / WID <= size.second)	// 宽度小高度大
    {
        EdgeWidth = size.first / WID - 1;
    }
    else// 宽度大高度小
    {
        EdgeWidth = size.second / HEI - 1;
    }
}


// 随机放置一个食物
void CSnakeView::SetFood()
{
    int a, b;							// 记录坐标
    srand((unsigned)time(NULL));		// 随机种子
    if (SnakeLen < WID * HEI / 2) {		// 当蛇长很小时
        do {
            a = rand() % WID + 1;		// 生成从1到WID的随机数
            b = rand() % HEI + 1;		// 生成从1到HEI的随机数
        } while (Board[a][b] != EMPTY);
    }
    else {// 当蛇很长时
        int R[WID * HEI / 2 + 1][2];	// 保存空位的坐标
        int t = 0;
        for (int i = 1; i <= WID; i++) {
            for (int j = 1; j <= HEI; j++) {
                if (Board[i][j] == EMPTY) {	// 寻找所有空位
                    R[t][0] = i;
                    R[t++][1] = j;
                }
            }
        }
        int r = rand() % t;				// 生成0到t-1的随机数
        a = R[r][0];
        b = R[r][1];					// 得到空位坐标
    }
    Board[a][b] = FOOD;					// 设置食物
    FoodPosX = a;
    FoodPosY = b;
    ShowPointOnTitle();					// 显示分数
}

// 绘制食物
void CSnakeView::DrawFood(ImageMatrix& img)
{
    CImage img;
    if (!LoadImageFromResource(hInstance, &img, FOODIMG, "PNG")) {
        MessageBox(hwnd, IMGERROR, NULL, NULL);		// 图片加载失败
        return;
    }
    TransparentPNG(&img);				// 透明处理
    img.Draw(							// 绘制食物
        compatibleDC,
        EdgeWidth * (FoodPosX - 1) + FoodPosX - FOODSIZE,
        EdgeWidth * (FoodPosY - 1) + FoodPosY - FOODSIZE,
        EdgeWidth + 2 * (FOODSIZE),
        EdgeWidth + 2 * (FOODSIZE),
        0, 0, img.GetWidth(), img.GetHeight()
    );								// 会失真
}

// 绘制棋盘
void CSnakeView::DrawBoard(ImageMatrix& img, Size& size)
{
    HPEN pen = CreatePen(PS_SOLID, 1, LINECOLOR);		// 边框
    HPEN Oldpen = (HPEN)SelectObject(compatibleDC, pen);
    HBRUSH br = (HBRUSH)::GetStockObject(NULL_BRUSH);	// 空心
    HBRUSH Oldbr = (HBRUSH)SelectObject(compatibleDC, br);
    for (int i = 0; i < WID; i++)											// 横向格子
        Rectangle(compatibleDC, EdgeWidth * i + i, 0, EdgeWidth * (i + 1) + i + 2, EdgeWidth * HEI + HEI + 1);
    for (int i = 0; i < HEI; i++)											// 纵向格子
        Rectangle(compatibleDC, 0, EdgeWidth * i + i, EdgeWidth * WID + WID + 1, EdgeWidth*(i + 1) + i + 2);
    SelectObject(compatibleDC, Oldpen);
    SelectObject(compatibleDC, Oldbr);
    DeleteObject(Oldpen);
    DeleteObject(pen);
    DeleteObject(Oldbr);
    DeleteObject(br);
}


// 在标题栏显示分数
void CSnakeView::ShowPointOnTitle()
{
    wchar_t buf[10];
    wnsprintf(buf, 9 * sizeof(int), POINTSTR, GamePoint());
    SetWindowText(hwnd, buf);
}

// 计算游戏得分
int CSnakeView::GamePoint()
{
    return (SnakeLen - DEFAULTLEN) * 100;	// 每吃一个食物得100分
}

// 根据Board进行蛇的绘制
void CSnakeView::DrawSnake(ImageMatrix& img)
{
    HPEN pen = CreatePen(PS_SOLID, 1, SNAKECOLOR);
    HBRUSH br = CreateSolidBrush(SNAKECOLOR);
    HPEN Oldpen = (HPEN)SelectObject(compatibleDC, pen);
    HBRUSH Oldbrush = (HBRUSH)SelectObject(compatibleDC, br);

    int t = 1;				// 计数
    int x = HeadPosX;		// 当前位置的坐标值
    int y = HeadPosY;
    int leftx, lefty;		// 左边的坐标值
    int rightx, righty;		// 右边的坐标值
    int upx, upy;			// 上边的坐标值
    int downx, downy;		// 下边的坐标值
    while (t <= SnakeLen) {

        // 边缘处理x
        switch (x)
        {
        case 1:				// 屏幕最左边
            leftx = WID;
            rightx = x + 1;
            break;
        case WID:			// 屏幕最右边
            leftx = x - 1;
            rightx = 1;
            break;
        default:			// 屏幕中间
            leftx = x - 1;
            rightx = x + 1;
            break;
        }
        upx = x;			// 上下边的x值不变
        downx = x;

        // 边缘处理y
        switch (y)
        {
        case 1:				// 屏幕最上边
            upy = HEI;
            downy = y + 1;
            break;
        case HEI:			// 屏幕最下边
            upy = y - 1;
            downy = 1;
            break;
        default:			// 屏幕中间
            upy = y - 1;
            downy = y + 1;
            break;
        }
        lefty = y;			// 左右边的y值不变
        righty = y;

        // 对上一截的位置进行判断
        if ((Board[leftx][lefty] == (t - 1)) && (t != 1))	// 左边是上一截
        {
            DrawHeadToRight(compatibleDC, EdgeWidth, x, y);			// 向右的头
        }
        if ((Board[rightx][righty] == (t - 1)) && (t != 1))	// 右边是上一截
        {
            DrawHeadToLeft(compatibleDC, EdgeWidth, x, y);			// 向左的头
        }
        if ((Board[upx][upy] == (t - 1)) && (t != 1))		// 上边是上一截
        {
            DrawHeadToDown(compatibleDC, EdgeWidth, x, y);			// 向下的头
        }
        if ((Board[downx][downy] == (t - 1)) && (t != 1))	// 下边是上一截
        {
            DrawHeadToUp(compatibleDC, EdgeWidth, x, y);			// 向上的头
        }

        // 对下一截的位置进行判断
        if (Board[leftx][lefty] == (t + 1))					// 左边是下一截
        {
            DrawHeadToRight(compatibleDC, EdgeWidth, x, y);			// 向右的头
            x = (x == 1) ? WID : x - 1;						// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        if (Board[rightx][righty] == (t + 1))				// 右边是下一截
        {
            DrawHeadToLeft(compatibleDC, EdgeWidth, x, y);			// 向左的头
            x = (x == WID) ? 1 : x + 1;						// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        if (Board[upx][upy] == (t + 1))						// 上边是下一截
        {
            DrawHeadToDown(compatibleDC, EdgeWidth, x, y);			// 向下的头
            y = (y == 1) ? HEI : y - 1;						// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        if (Board[downx][downy] == (t + 1))					// 下边是下一截
        {
            DrawHeadToUp(compatibleDC, EdgeWidth, x, y);			// 向上的头
            y = (y == HEI) ? 1 : y + 1;						// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        // 上下左右都找不到下一截，说明此处是末尾
        break;
    }
    SelectObject(compatibleDC, Oldpen);
    SelectObject(compatibleDC, Oldbrush);
    DeleteObject(Oldpen);
    DeleteObject(pen);
    DeleteObject(Oldbrush);
    DeleteObject(br);
}

// 根据Board及Direction进行蛇运动的计算
// 仅当需要输出彩蛋时返回False
int CSnakeView::SnakeGo()
{
    // 分析
    int x = HeadPosX;		// 头部的坐标值
    int y = HeadPosY;
    int leftx, lefty;		// 左边的坐标值
    int rightx, righty;		// 右边的坐标值
    int upx, upy;			// 上边的坐标值
    int downx, downy;		// 下边的坐标值
    bool HeadEatTail = false;	// 解决首尾相撞

    // 处理头部

    // 边缘处理x
    switch (x)
    {
    case 1:					// 屏幕最左边
        leftx = WID;
        rightx = x + 1;
        break;
    case WID:				// 屏幕最右边
        leftx = x - 1;
        rightx = 1;
        break;
    default:				// 屏幕中间
        leftx = x - 1;
        rightx = x + 1;
        break;
    }
    upx = x;				// 上下边的x值不变
    downx = x;
    // 边缘处理y
    switch (y)
    {
    case 1:					// 屏幕最上边
        upy = HEI;
        downy = y + 1;
        break;
    case HEI:				// 屏幕最下边
        upy = y - 1;
        downy = 1;
        break;
    default:				// 屏幕中间
        upy = y - 1;
        downy = y + 1;
        break;
    }
    lefty = y;				// 左右边的y值不变
    righty = y;

    // 处理蛇头移动
    switch (Direction)
    {
    case TOLEFT:
        // 对左一格的物体进行判断
        switch (Board[leftx][lefty])
        {
        case EMPTY:							// 空格子
            Board[leftx][lefty] = 1;		// 往左边方向设置一个蛇头
            break;
        case FOOD:							// 食物
            Board[leftx][lefty] = 1;		// 往左边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)	// 如果满屏爆表了
            {
                GameOver();					// 调用GameOver()结束定时器
                return GOFULLSCREEN;		// 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:							// 碰到身体
            if (Board[leftx][lefty] != SnakeLen)
            {
                GameOver();					// 游戏结束
                return GOGAMEOVER;			// 返回
                break;
            }
            else
            {
                Board[leftx][lefty] = 1;	// 碰到尾巴不算，往左边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = leftx;					// 更新蛇头位置
        HeadPosY = lefty;
        break;
    case TORIGHT:
        // 对右一格的物体进行判断
        switch (Board[rightx][righty])
        {
        case EMPTY:							// 空格子
            Board[rightx][righty] = 1;		// 往右边方向设置一个蛇头
            break;
        case FOOD:							// 食物
            Board[rightx][righty] = 1;		// 往右边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)	// 如果满屏爆表了
            {
                GameOver();					// 调用GameOver()结束定时器
                return GOFULLSCREEN;		// 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:							// 碰到身体
            if (Board[rightx][righty] != SnakeLen)
            {
                GameOver();					// 游戏结束
                return GOGAMEOVER;			// 返回
                break;
            }
            else
            {
                Board[rightx][righty] = 1;	// 碰到尾巴不算，往右边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = rightx;					// 更新蛇头位置
        HeadPosY = righty;
        break;
    case TOUP:
        // 对上一格的物体进行判断
        switch (Board[upx][upy])			// 对该方向的下一格进行判断
        {
        case EMPTY:							// 空格子
            Board[upx][upy] = 1;			// 往上边方向设置一个蛇头
            break;
        case FOOD:							// 食物
            Board[upx][upy] = 1;			// 往上边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)	// 如果满屏爆表了
            {
                GameOver();					// 调用GameOver()结束定时器
                return GOFULLSCREEN;		// 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:							// 碰到身体
            if (Board[upx][upy] != SnakeLen)
            {
                GameOver();					// 游戏结束
                return GOGAMEOVER;			// 返回
                break;
            }
            else
            {
                Board[upx][upy] = 1;		// 碰到尾巴不算，往上边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = upx;						// 更新蛇头位置
        HeadPosY = upy;
        break;
    case TODOWN:
        // 对下一格的物体进行判断
        switch (Board[downx][downy])		// 对该方向的下一格进行判断
        {
        case EMPTY:							// 空格子
            Board[downx][downy] = 1;		// 往下边方向设置一个蛇头
            break;
        case FOOD:							// 食物
            Board[downx][downy] = 1;		// 往下边方向设置一个蛇头
            if (SnakeLen == WID * HEI - 1)	// 如果满屏爆表了
            {
                GameOver();					// 调用GameOver()结束定时器
                return GOFULLSCREEN;		// 退出函数，输出彩蛋
            }
            Eaten = true;
            break;
        default:							// 碰到身体
            if (Board[downx][downy] != SnakeLen)
            {
                GameOver();					// 游戏结束
                return GOGAMEOVER;			// 返回
                break;
            }
            else
            {
                Board[downx][downy] = 1;	// 碰到尾巴不算，往下边方向设置一个蛇头
                // 上面这步操作把尾巴覆盖了，这是为了避免遍历时在尾巴处减一
                HeadEatTail = true;
                break;
            }
        }
        HeadPosX = downx;					// 更新蛇头位置
        HeadPosY = downy;
        break;
    default:
        return GOFULLSCREEN;				// 没有方向？这不科学。就当做输出彩蛋吧
    }

    // 其余部分的移动（x,y为原来蛇的第一截，新蛇的第二截）
    int t = 1;				// 原来蛇的第一截，新蛇的第二截

    while (t <= SnakeLen) {
        // 边缘处理x
        switch (x)
        {
        case 1:				// 屏幕最左边
            leftx = WID;
            rightx = x + 1;
            break;
        case WID:			// 屏幕最右边
            leftx = x - 1;
            rightx = 1;
            break;
        default:			// 屏幕中间
            leftx = x - 1;
            rightx = x + 1;
            break;
        }
        upx = x;			// 上下边的x值不变
        downx = x;

        // 边缘处理y
        switch (y)
        {
        case 1:				// 屏幕最上边
            upy = HEI;
            downy = y + 1;
            break;
        case HEI:			// 屏幕最下边
            upy = y - 1;
            downy = 1;
            break;
        default:			// 屏幕中间
            upy = y - 1;
            downy = y + 1;
            break;
        }
        lefty = y;			// 左右边的y值不变
        righty = y;

        // 对下一截的位置进行判断
        if (Board[leftx][lefty] == (t + 1))		// 左边是下一截
        {
            Board[x][y]++;						// 蛇移动
            x = (x == 1) ? WID : x - 1;			// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        if (Board[rightx][righty] == (t + 1))	// 右边是下一截
        {
            Board[x][y]++;						// 蛇移动
            x = (x == WID) ? 1 : x + 1;			// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        if (Board[upx][upy] == (t + 1))			// 上边是下一截
        {
            Board[x][y]++;						// 蛇移动
            y = (y == 1) ? HEI : y - 1;			// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        if (Board[downx][downy] == (t + 1))		// 下边是下一截
        {
            Board[x][y]++;						// 蛇移动
            y = (y == HEI) ? 1 : y + 1;			// 下一截蛇的身体
            t++;								// 计数+1
            continue;							// 跳出
        }
        // 如果能找到下一截，说明此处不是末尾，此时 x,y 都变成了刚才找到的下一截的位置
        // 上下左右都找不到下一截，说明此处是末尾，此时 x,y 仍然是当前位置
        if (Eaten) {
            Board[x][y] = t + 1;				// 尾巴延长
            SnakeLen++;							// 蛇长+1
            Eaten = false;
            SetFood();							// 再放置一个食物
        }
        else
        {
            if (HeadEatTail)
                Board[x][y] = t + 1;			// 首尾相接，把被头覆盖掉的尾巴补回
            else
                Board[x][y] = EMPTY;			// 清除末尾
        }
        break;
    }
    return GOSUCCESS;
}

//消息处理

void CSnakeView::OnKeyDown(UINT nChar)
{
    // TODO:  在此添加消息处理程序代码和/或调用默认值
    switch (nChar)
    {
    case VK_RETURN:
        nChar = VK_SPACE;						// 回车=空格
    case VK_SPACE:
        if (StartNew) {
            GameStart();
        }
        else {
            if (isPause)
            {
                SetTimer(hwnd, 1, Speed, NULL);
                isPause = false;				// 取消暂停
            }
            else
            {
                KillTimer(hwnd, 1);

                HDC pDC = ::GetDC(hwnd);
                gdi.printText(150, 100, GAMEPAUSE, 4, TEXTFONT, TEXTSIZE, FONTCOLOR); // 显示暂停文字
                ReleaseDC(hwnd, pDC);

                isPause = true;					// 暂停游戏
            }
        }
        break;
    case VK_UP:
        if (Direction != TODOWN && Using == false)
        {
            // 往下走不能突然向上,被占用时也不能更改方向
            Direction = TOUP;					// 方向向上
            Using = true;						// 开始占用
        }
        break;
    case VK_DOWN:
        if (Direction != TOUP && Using == false)
        {
            // 往上走不能突然向下,被占用时也不能更改方向
            Direction = TODOWN;					// 方向向下
            Using = true;						// 开始占用
        }
        break;
    case VK_LEFT:
        if (Direction != TORIGHT && Using == false)
        {
            // 往右走不能突然向左,被占用时也不能更改方向
            Direction = TOLEFT;					// 方向向左
            Using = true;						// 开始占用
        }
        break;
    case VK_RIGHT:
        if (Direction != TOLEFT && Using == false)
        {
            // 往左走不能突然向右,被占用时也不能更改方向
            Direction = TORIGHT;				// 方向向右
            Using = true;						// 开始占用
        }
        break;
    default:
        break;
    }
}

void CSnakeView::OnTimer(UINT_PTR nIDEvent)
{
    Using = false;					// 解除占用

    // 定时器只负责调用蛇运动的计算，接着在画图消息里重绘
    int result = SnakeGo();			// 分析蛇（吃了食物自动SetFood）

    RECT DRECT;
    switch (result)
    {
    case GOSUCCESS:
        GetClientRect(hwnd, &DRECT);

        // 由 InvalidateRect 发送 WM_PAINT 消息
        // WM_PAINT 消息调用 OnDraw 来绘图
        InvalidateRect(hwnd, DRECT, FALSE);
        break;
    case GOGAMEOVER:
        // SnakeGo 会调用 GameOver
        break;
    case GOFULLSCREEN:
        MessageBox(hwnd, YOUWIN, NULL, NULL);	// 输出彩蛋
        break;
    }
}

// 菜单：开始/暂停
void CSnakeView::OnStart()
{
    // TODO:  在此添加命令处理程序代码
    if (StartNew) {
        GameStart();
    }
    else {
        if (isPause)
        {
            SetTimer(hwnd, 1, Speed, NULL);
            isPause = false;				// 取消暂停
        }
        else
        {
            KillTimer(hwnd, 1);

            HDC pDC = ::GetDC(hwnd);
            gdi.printText(150, 100, GAMEPAUSE, 4, TEXTFONT, TEXTSIZE, FONTCOLOR); // 显示暂停文字
            ReleaseDC(hwnd, pDC);

            isPause = true;					// 暂停游戏
        }
    }
}


void CSnakeView::OnFastspeed()
{
    Speed = FAST;
    if (!StartNew) {							// 游戏状态下，重置定时器
        KillTimer(hwnd, 1);
        isPause = false;
        SetTimer(hwnd, 1, Speed, NULL);
    }
}

void CSnakeView::OnSlowspeed()
{
    Speed = SLOW;
    if (!StartNew) {							// 游戏状态下，重置定时器
        KillTimer(hwnd, 1);
        isPause = false;
        SetTimer(hwnd, 1, Speed, NULL);
    }
}

