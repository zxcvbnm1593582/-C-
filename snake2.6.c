/*************************************************************************



            > File Name: 贪吃蛇qwq          demo 2.0        update : 彻底消除闪动, 不再采用数组标记位置


        > Author: ewoifuoi
 ************************************************************************/
#define _CRT_SECURE_NO_WARNINGS //定义这个宏是为了关闭 Visual Studio 编译器对一些旧版 C 标准函数（如 scanf）的安全警告，避免编译时被提示报错
#include<stdio.h>
#include<stdlib.h> //引入标准库头文件，提供 rand()（随机数）、malloc()（内存分配）等函数
#include<string.h>
#include<time.h> //引入时间处理头文件，用于获取系统时间来初始化随机数种子
#include<math.h>
#include<windows.h> //引入 Windows 系统 API 头文件，提供控制台窗口控制、光标移动等系统级功能
#include<conio.h> //引入控制台输入输出头文件，提供 _getch() 函数，用于无回显读取键盘输入（适合贪吃蛇的方向键控制）

//定义结构体并简化写法其名称 
typedef struct Point {
    int x, y;
} point;

typedef struct Snake {
    int length;
    int speed;
    int color;
    int dead;
    point node[101]; //蛇身的坐标点数组，最多支持 100 节身体（加 1 个头部） 
} snake;

typedef struct Food {
    point location; // 食物的坐标点 
        int type; // 食物的类型（比如普通食物、特殊食物，用于不同得分或效果） 
} food;

// 定义一些全局变量 
food f;
snake s;
int mark[30][15]; // 30×15 的二维数组，作为游戏地图的标记矩阵，用来记录每个位置是否有蛇身或食物 
char in_put;
int dir;
int mode = 0;
int score;
int h_score;


void HideCursor(); // 隐藏控制台光标，避免游戏画面闪烁 
void goto_xy(int x, int y); //将控制台光标移动到指定坐标(x, y)，用于精准绘制蛇身和食物 
void set_console_color(unsigned short color_index); // 设置控制台文本颜色，让蛇和食物显示不同颜色 
void hello(); // 游戏启动时的欢迎界面，显示标题、玩法说明等 
void init(); // 游戏初始化函数，负责重置地图、蛇、食物、分数等状态 
void input(); // 处理键盘输入，监听方向键并更新蛇的移动方向 
void move(); // 蛇的移动逻辑，更新蛇头和身体的坐标，处理碰撞检测 
void die(); // 蛇死亡后的处理，显示游戏结束界面、得分，并询问是否重新开始 
void make_boundary(); // 绘制游戏地图的边界，让玩家看到可移动范围 
void init_snake(); // 初始化蛇的初始位置、长度和方向 
void print(int x, int y, int color, int w); // 在指定坐标(x, y) 处，用指定颜色和字符绘制内容（比如蛇身用 ■，食物用 ●） 
void make_food(); // 在地图上随机生成食物，并确保食物不会出现在蛇身上
void eat(); // 处理蛇吃食物的逻辑：增加蛇的长度、得分，并生成新的食物 

int main() {
    hello(); //调用函数通常这类函数是用来显示欢迎界面、游戏标题或操作说明的
    init(); //完成游戏的初始化工作
    //无限循环
    while (1) {
        input(); //处理用户输入的函数
        move(); //处理蛇的移动的函数
        //判断蛇是否已经死亡或者撞墙
        if (s.dead == 1) {
            die(); //处理死亡后的逻辑
            //用户选择重新开始的状态标记
            if (s.dead == 3) {
                system("CLS"); //调用系统命令 CLS 来清空控制台屏幕，为重新初始化游戏做准备
                init(); //再次调用初始化函数，重置游戏状态，重新开始一局
                continue;
            }
            break;
        }
    }
    return 0;
}

void hello() {
    srand(time(0)); //time(0) 获取当前系统时间，作为随机数生成器的种子；初始化随机数生成器，确保每次运行游戏时，后续的 rand() 都能产生不同的随机序列
    srand() 
    printf("\n\n\n\n\n\t\t\t\t贪吃蛇");
    printf("qwq");
    set_console_color(8); //调用自定义函数设置控制台文字颜色为灰色（颜色值 8）
    printf("\tdemo 2.0");
    set_console_color(7); //把控制台文字颜色恢复为默认的白色（颜色值 7
    printf("\n\n\n\n\t\t\t\t  输入任意键开始游戏...\n\n\n\n\n\n\n\n\n");
    //进入一个无限循环，用来持续检测用户输入，同时展示动态效果
    while (1) {
        //_kbhit() 是一个 Windows 平台的函数，用来检测是否有键盘按键被按下
        if (_kbhit()) {
            system("CLS"); //调用系统命令 CLS 清空整个控制台屏幕，为进入游戏主界面做准备
            return;
        }
        HideCursor(); //调用自定义函数隐藏控制台的光标，避免光标闪烁影响界面美观
        int qwq = rand() % 7 + 1; //生成一个 1 到 7 之间的随机整数，赋值给变量 qwq
        goto_xy(38, 5);  // 调用自定义函数 goto_xy()，将控制台光标移动到坐标(38, 5) 的位置；这是为了在固定位置刷新表情字符
        printf("   ");
        goto_xy(38, 5); //再次将光标移动到 (38, 5) 这个固定位置，准备重新输出彩色的 qwq
        set_console_color(qwq); //用第 15 行生成的随机颜色值 qwq 来设置控制台文字颜色
        printf("qwq");
        set_console_color(7); //把文字颜色恢复为默认的白色
        goto_xy(34, 9); //将光标移动到坐标 (34, 9) 的位置，准备显示提示文字
        printf("输入任意键开始游戏...");
        Sleep(500); //让程序暂停 500 毫秒（0.5 秒），控制动态效果的节奏
        goto_xy(34, 9); //再次将光标移动到 (34, 9) 位置
        printf("                     ");
        Sleep(200); //让程序暂停 200 毫秒（0.2 秒），配合上一步实现提示文字的闪烁
    }
    return;
}

void init() {
    make_boundary(); //调用自定义函数，用来绘制游戏的边界
    dir = 1; //初始化蛇的初始移动方向
    init_snake(); //调用初始化蛇的函数，完成蛇的初始设置：设置蛇头的初始位置；生成初始长度的蛇身；在界面上绘制出整条蛇
    memset(mark, 0, sizeof(mark)); //memset 是 C 语言标准库函数，作用是把 mark 数组的所有元素都设置为0；mark 一般是一个二维数组，用来标记蛇身占据的位置，避免蛇头撞到自己
    make_food(); //调用生成食物的函数，在游戏区域内随机生成一个食物（比如字符 *），并且确保食物不会出现在蛇身上
    return;
}

void input() {
    //_kbhit() 是 Windows 平台的函数，用来检测是否有键盘按键被按下；如果检测到有按键，就进入这个 if 块处理输入
    if (_kbhit()) {
        in_put = _getch(); //_getch() 会读取按下的按键字符，并且不会在控制台显示。读取到的按键值会被赋值给变量 in_put
        //根据 in_put 的值，进入不同的 case 分支，处理对应的按键
        switch (in_put) {
            //'w','s','a','d'分别控制上下左右
            //并且为防止蛇头直接碰到蛇身，在一个方向上移动时只能往相垂直的方向转
        case 'a': if (dir == 3 || dir == 4)dir = 2; break;
        case 'd': if (dir == 3 || dir == 4)dir = 1; break;
        case 'w': if (dir == 1 || dir == 2)dir = 3; break;
        case 's': if (dir == 1 || dir == 2)dir = 4; break;
        }
    }
    return;
}

//处理蛇的移动和状态更新
void move() {
    print(s.node[s.length].x, s.node[s.length].y, s.color, 0); //在蛇尾的坐标位置，用 “擦除模式”（参数 0）输出空格，消除上一帧的蛇尾，为移动做准备
    //启动一个从蛇尾到蛇头前一节的倒序循环
    for (int i = s.length; i >= 1; i--) {
        int xx = s.node[i].x; int yy = s.node[i].y;
        mark[s.node[i].x][s.node[i].y] = 0; //在 mark 数组中，把当前节点的旧位置标记为 0，表示该位置不再有蛇身
        s.node[i].x = s.node[i - 1].x; s.node[i].y = s.node[i - 1].y;
        mark[s.node[i].x][s.node[i].y] = 1; //在 mark 数组中，把当前节点的新位置标记为 1，表示该位置现在有蛇身
        print(s.node[i].x, s.node[i].y, s.color, 1);
    }
    //根据全局方向变量 dir，更新蛇头的位置
    switch (dir) {
    case 1: s.node[0].x++; break; //dir == 1（右方向）：蛇头的 x 坐标加 1，向右移动
    case 2: s.node[0].x--; break; //dir == 2（左方向）：蛇头的 x 坐标减 1，向左移动
    case 3: s.node[0].y--; break; //dir == 3（上方向）：蛇头的 y 坐标减 1，向上移动
    case 4: s.node[0].y++; break; //dir == 4（下方向）：蛇头的 y 坐标加 1，向下移
    }
    //在新的蛇头位置，用 “绘制模式” 输出蛇头字符，更新显示
    print(s.node[0].x, s.node[0].y, s.color, 1);
    //判断蛇头坐标是否与食物坐标重合，即是否吃到食物
    if (s.node[0].x == f.location.x && s.node[0].y == f.location.y) {
        eat();  //如果吃到食物，调用 eat() 函数，处理蛇变长、生成新食物等逻辑
    }
    //判断蛇头是否撞到游戏边界（x=0、x=29、y=0、y=14 是边界坐标
    if (s.node[0].x == 0 || s.node[0].y == 0 || s.node[0].x == 29 || s.node[0].y == 14) {
        s.dead = 1; //如果撞墙，设置 s.dead = 1，标记蛇死亡
        return;
    }
    //通过 mark 数组检查蛇头新位置是否被蛇身占据，即是否撞到自己
    if (mark[s.node[0].x][s.node[0].y] == 1) {
        s.dead = 1; //如果撞到自己，同样设置 s.dead = 1
        return;
    }
    Sleep(s.speed); //暂停 s.speed 毫秒，控制蛇的移动速度，值越小速度越快

    return;
}

void die() {
    FILE* fp; //声明一个文件指针 fp，用于读取和写入最高分文件
    //尝试以 “读写模式” 打开最高分文件 snake_h_score.txt
    if ((fp = fopen("snake_h_score.txt", "r+")) == NULL) {
        fp = fopen("snake_h_score.txt", "w+");
    }
    fscanf(fp, "%d", &h_score);
    s.dead = 1; //设置游戏状态为 “死亡”（虽然在进入此函数前，s.dead 已经是 1，这里是冗余的确认）
    //判断当前游戏得分 score 是否超过了历史最高分 h_score
    if (score > h_score) {
        fclose(fp);
        fp = fopen("snake_h_score.txt", "w+"); //以 “写入 + 读取模式” 重新打开文件，覆盖原有内容
        fprintf(fp, "%d", score);
        h_score = score; //更新内存中的 h_score 变量，使其等于当前得分

    }
    system("CLS"); //调用系统命令清空控制台屏幕，准备显示死亡界面
    printf("\n\n\n\n\n\n\n\t\t\t\t\t    你死了...\n\n");
    printf("\n\n\t\t\t\t\t当前分数为 : %d\n\n", score);
    printf("\t\t\t\t\t历史最高分数为 : %d\n\n", h_score);
    printf("\n\t\t\t\t  按 r键重来 , 按其余键结束游戏\n\n\n\n\n\n\n\n\n");
    char c = _getch(); //读取用户按下的按键（不显示在控制台），并赋值给变量 c
    if (c == 'r') {
        s.dead = 3; //如果按下 r，设置 s.dead = 3，这是 “重新开始” 的状态标记
    }
    else {
        s.dead = 2; //设置 s.dead = 2，这是 “退出游戏” 的状态标记
    }
    fclose(fp);
    return;
}

void goto_xy(int x, int y)//定位光标位置到指定坐标
{
    HANDLE hOut; //定义一个 HANDLE 类型的变量 hOut，用来表示控制台的输出设备句柄
    hOut = GetStdHandle(STD_OUTPUT_HANDLE); //获取标准输出设备（控制台窗口）的句柄，后续操作都需要通过它来进行
    COORD pos = { x,y }; //COORD 是 Windows API 定义的坐标结构体，这里用传入的 x 和 y 初始化一个坐标点
    SetConsoleCursorPosition(hOut, pos); //调用系统 API，将光标移动到 pos 指定的坐标位置
}

void set_console_color(unsigned short color_index)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_index); //这是一个 Windows API 函数，用来设置控制台输出文字的颜色和背景色
}
//定义隐藏光标函数
void HideCursor()
{
    CONSOLE_CURSOR_INFO cursor; //定义一个 CONSOLE_CURSOR_INFO 结构体变量，用来存储光标的信息
    cursor.bVisible = FALSE; //将光标可见性设置为 FALSE，即隐藏光标
    cursor.dwSize = sizeof(cursor); //设置光标的大小，这里用结构体大小作为占位值，实际隐藏时此值不影响效果
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取控制台输出的句柄
    SetConsoleCursorInfo(handle, &cursor); //调用系统 API，将新的光标设置应用到控制台，实现光标隐藏
void make_boundary() {
    //启动第一个循环，i 从 0 到 30，用来绘制上下两条水平边框
    for (int i = 0; i <= 30; i++) {
        goto_xy(i, 0); //将光标定位到坐标 (i, 0)，也就是屏幕最顶行的第 i 个位置
        printf("X");
        goto_xy(i, 15); //将光标定位到坐标 (i, 15)，也就是屏幕最底行的第 i 个位置
        printf("X");
    }
    //启动第二个循环，i 从 0 到 15，用来绘制左右两条垂直边框
    for (int i = 0; i <= 15; i++) {
        goto_xy(0, i); //将光标定位到坐标 (0, i)，也就是屏幕最左列的第 i 个位置
        printf("X");
        goto_xy(30, i); //将光标定位到坐标 (30, i)，也就是屏幕最右列的第 i 个位置
        printf("X");
    }
    return;
}

void init_snake() {
    s.length = 5;
    s.dead = 0;
    s.speed = 100;
    s.color = 7;
    s.node[0].x = 6; s.node[0].y = 6;
    mark[s.node[0].x][s.node[0].y] = 1;//在 mark 标记数组中，将蛇头的位置标记为 1，表示该位置有蛇身
    goto_xy(6, 6); //将光标定位到蛇头的坐标 (6, 6)
    printf("O");
    s.node[1].x = 5; s.node[1].y = 6; //设置第 1 号蛇身节点的坐标为 (5, 6)
    mark[s.node[1].x][s.node[1].y] = 1; //在 mark 数组中标记该位置为有蛇身
    goto_xy(5, 6); //定位光标到该蛇身节点的坐标
    printf("O");
    s.node[2].x = 4; s.node[2].y = 6; //设置第 2 号蛇身节点的坐标为 (4, 6)
    mark[s.node[2].x][s.node[2].y] = 1; //标记该位置为有蛇身
    goto_xy(4, 6); //定位光标到该蛇身节点的坐标
    printf("O");
    s.node[3].x = 3; s.node[3].y = 6; //设置第 3 号蛇身节点的坐标为 (3, 6)
    mark[s.node[3].x][s.node[3].y] = 1; //标记该位置为有蛇身
    goto_xy(3, 6); //定位光标到该蛇身节点的坐标
    printf("O");
    s.node[4].x = 2; s.node[4].y = 6; //设置第 4 号蛇身节点的坐标为 (2, 6)
    mark[s.node[4].x][s.node[4].y] = 1; //标记该位置为有蛇身
    goto_xy(2, 6); //定位光标到该蛇身节点的坐标
    printf("O");
    s.node[5].x = 1; s.node[5].y = 6; //设置第 5 号蛇身节点的坐标为 (1, 6)
    mark[s.node[5].x][s.node[5].y] = 1; //标记该位置为有蛇身
    goto_xy(1, 6); //定位光标到该蛇身节点的坐标
    printf("O");
    return;
}

void print(int x, int y, int color, int w) {
    HideCursor(); //隐藏光标，避免绘制时闪烁
    set_console_color(color); //设置绘制时的文字颜色
    goto_xy(x, y); //将光标定位到指定坐标 (x,y)
    //根据样式参数 w，选择绘制不同的字符
    switch (w) {
    case 0: {
        printf(" "); //样式 0：输出空格，用于 “擦除” 已绘制的内容
        break;
    }
    case 1: {
        printf("O"); //样式 1：输出字符 0，用于绘制蛇身
        break;
    }
    case 2: {
        printf("X"); //样式 2：输出字符 X，用于绘制游戏边界
        break;
    }
    case 3: {
        printf("♥"); //样式 3：输出字符 ♠，用于绘制食物
        break;
    }
    }
    set_console_color(7); //绘制完成后，将颜色恢复为默认白色（7）
    return;
}
void make_food() {
    srand(time(0)); //用当前时间初始化随机数生成器，确保每次生成的食物位置都不同
    f.location.x = rand() % 28 + 1; //生成食物的 x 坐标：1 到 29 之间（避免出现在左右边界上）
    f.location.y = rand() % 13 + 1; //生成食物的 y 坐标：1 到 13 之间（避免出现在上下边界上）
    f.type = rand() % 4; //生成食物的类型（0-3），不同类型可能对应不同的分数和颜色
    int COLOR[4] = { 7 ,5, 2, 11 }; //定义一个颜色数组，为不同类型的食物设置不同颜色
    print(f.location.x, f.location.y, COLOR[f.type], 3);
    return;
}

void eat() {
    int TYPE_SCORE[4] = { 10, 50, 10, 10 }; //定义不同类型食物对应的得分
    print(f.location.x, f.location.y, 7, 0);
    mode = f.type; //记录当前吃到的食物类型
    score += TYPE_SCORE[f.type]; //根据食物类型，增加对应的分数到总得分
    //吃到特殊食物
    if (f.type == 1) {
        s.length *= 2;
    }
    //吃到普通食物
    else {
        s.length++;
    }
    make_food();
    return;
}