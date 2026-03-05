/*************************************************************************
            > File Name: 贪吃蛇qwq          demo 2.0        update : 彻底消除闪动, 不再采用数组标记位置
            > Author: ewoifuoi
            > Update: VS2022适配+纯数字0123皮肤+调试信息打印
  ************************************************************************/
#define _CRT_SECURE_NO_WARNINGS  // 关闭VS的C语言安全函数警告
#include<stdio.h>               // 标准输入输出
#include<stdlib.h>              // 通用工具（rand/srand/memset/system等）
#include<string.h>              // 字符串操作
#include<time.h>                // 时间函数（time(0)给rand设随机种子）
#include<math.h>                // 数学函数）
#include<windows.h>             // Windows系统API（控制台光标/颜色/坐标控制）
#include<conio.h>               // 控制台输入（_kbhit()检测按键、_getch()无回显读按键）

typedef enum {                  // 12-16：定义蛇的皮肤类型枚举
    SKIN_0 = 0,                 // 皮肤0（字符'0'）
    SKIN_1 = 1,                 // 皮肤1（字符'1'）
    SKIN_2 = 2,                 // 皮肤2（字符'2'）
    SKIN_3 = 3                  // 皮肤3（字符'3'）
} SkinType;

const char skinChars[4] = { '0', '1', '2', '3' };  // 皮肤对应的显示字符
const char* skinNames[4] = { "0", "1", "2", "3" }; // 皮肤名（菜单显示用）

const char* foodTypeNames[4] = { "普通食物", "双倍长度食物", "基础食物", "加分食物" }; // 食物类型名称（调试显示）
typedef struct Point {          // 坐标结构体
    int x, y;                   // x=横向坐标，y=纵向坐标
} point;

typedef struct Snake {          // 蛇的核心属性结构体
    int length;                 // 蛇身长度
    int speed;                  // 移动速度
    int color;                  // 显示颜色
    int dead;                   // 死亡状态
    point node[101];            // 蛇身节点数组（101节防止溢出）
    SkinType skin;              // 皮肤类型
} snake;

typedef struct Food {           // 食物结构体
    point location;             // 食物坐标
    int type;                   // 食物类型：0=普通，1=双倍长度，2=基础，3=加分
} food;

food f;                         // 全局食物对象
snake s;                        // 全局蛇对象
int mark[30][15];               // 地图标记数组：0=空，1=蛇身
char in_put;                    // 按键输入缓存
int dir;                        // 移动方向：1=右，2=左，3=上，4=下
int mode = 0;                   // 食物类型缓存
int score;                      // 当前分数
int h_score;                    // 历史最高分

void showSkinMenu();            // 皮肤选择菜单
char getSkinChar(SkinType type);// 根据皮肤类型返回显示字符
void printDebugInfo();          // 打印调试信息
void HideCursor();              // 隐藏控制台光标
void goto_xy(int x, int y);     // 设置控制台光标位置
void set_console_color(unsigned short color_index); // 设置字体颜色
void hello();                   // 游戏开始欢迎界面
void init();                    // 游戏初始化
void input();                   // 按键输入处理
void move();                    // 蛇的移动逻辑
void die();                     // 死亡处理
void make_boundary();           // 绘制边界
void init_snake();              // 初始化蛇
void print(int x, int y, int color, int w); // 控制台打印字符空格和食物
void make_food();               // 生成食物
void eat();                     // 吃食物逻辑

int main() {                    //程序入口函数
    srand(time(0));             //显示皮肤选择菜单
    hello();                    //显示欢迎界面
    init();                     //初始化游戏（边界、蛇、食物）
    while (1) {                 //游戏主循环（无限循环，直到死亡）
        input();                //检测并处理按键
        move();                 //蛇移动
        printDebugInfo();       //打印调试信息
        if (s.dead == 1) {      //判断是否死亡
            die();              //执行死亡逻辑
            if (s.dead == 3) {  //如果按r键重来
                system("CLS");  //清屏
                showSkinMenu(); //重新选皮肤
                hello();        //重新显示欢迎界面
                init();         //重新初始化
                continue;       //跳过本次循环，进入下一轮
            }
            break;              //否则退出主循环，结束游戏
        }
    }
    return 0;                   //程序正常退出
}

void printDebugInfo() {
    goto_xy(35, 1);             //光标移到右侧调试区
    set_console_color(15);      //设置字体为白色
    printf("===== 调试信息 =====\n");
    goto_xy(35, 2);
    printf("蛇头坐标：X=%d, Y=%d       \n", s.node[0].x, s.node[0].y); //打印蛇头坐标
    goto_xy(35, 3);
    printf("当前分数：%d              \n", score); //打印分数
    goto_xy(35, 4);
    printf("蛇身长度：%d              \n", s.length); //打印长度
    goto_xy(35, 5);
    printf("食物类型：%s             \n", foodTypeNames[f.type]); //打印食物类型
    goto_xy(35, 6);
    printf("食物坐标：X=%d, Y=%d       \n", f.location.x, f.location.y); //食物坐标
    goto_xy(35, 7);
    const char* dirName = "";
    switch (dir) {              //转换方向码为文字
    case 1: dirName = "右"; break;
    case 2: dirName = "左"; break;
    case 3: dirName = "上"; break;
    case 4: dirName = "下"; break;
    }
    printf("移动方向：%s              \n", dirName);
    printf("====================\n");
    set_console_color(7);       //恢复默认灰色字体
}

void showSkinMenu() {
    system("CLS");              //清屏
    HideCursor();               //隐藏光标避免闪烁
    int select = 0;             //默认选中第0个皮肤
    while (1) {                 //菜单循环
        goto_xy(25, 5);
        set_console_color(14);  //黄色字体
        printf("===== 选择数字皮肤 =====\n");
        set_console_color(7);   //恢复灰色
        for (int i = 0; i < 4; i++) { //遍历4个皮肤
            goto_xy(30, 7 + i);
            if (i == select) {  //选中项标绿
                set_console_color(10);
                printf("> %s\n", skinNames[i]);
            }
            else {              //未选中项灰色
                set_console_color(7);
                printf("  %s\n", skinNames[i]);
            }
        }
        goto_xy(25, 12);
        set_console_color(8);   //灰色提示文字
        printf("↑↓键选择 | 回车键确认\n");
        set_console_color(7);
        if (_kbhit()) {         //检测是否有按键
            in_put = _getch();  //读取按键
            switch (in_put) {
            case 72: select = (select - 1 + 4) % 4; break; //上键（ASCII 72）：选上一个
            case 80: select = (select + 1) % 4; break;     //下键（ASCII 80）：选下一个
            case 13: s.skin = (SkinType)select; system("CLS"); return; // 回车（ASCII 13）：确认选择
            }
        }
        Sleep(50);              //设置50ms延迟来降低CPU占用
    }
}

char getSkinChar(SkinType type) {
    return skinChars[type];//根据皮肤类型返回对应字符
}

void hello() {
    printf("\n\n\n\n\n\t\t\t\t贪吃蛇");
    printf("qwq");
    set_console_color(8);
    printf("\tdemo 2.0");
    set_console_color(7);
    printf("\n\n\n\n\t\t\t\t  输入任意键开始游戏...\n\n\n\n\n\n\n\n\n");
    while (1) {                 //动画循环
        if (_kbhit()) {         //检测到按键
            system("CLS");      //清屏
            return;             //退出欢迎界面
        }
        HideCursor();           //隐藏光标
        int qwq = rand() % 7 + 1; //随机颜色码（1-7）
        goto_xy(38, 5);
        printf("   ");          //清空原有字符
        goto_xy(38, 5);
        set_console_color(qwq); //设置随机颜色
        printf("qwq");          //显示qwq
        set_console_color(7);   //恢复默认
        goto_xy(34, 9);
        printf("输入任意键开始游戏...");
        Sleep(500);             //显示500ms
        goto_xy(34, 9);
        printf("                     "); //清空提示
        Sleep(200);             //隐藏200ms闪烁效果
    }
    return;
}

void init() {
    make_boundary();            //绘制边界
    dir = 1;                    //默认向右移动
    init_snake();               //初始化蛇
    memset(mark, 0, sizeof(mark)); //清空标记数组
    for (int i = 0; i <= s.length; i++) { //标记蛇身位置
        mark[s.node[i].x][s.node[i].y] = 1;
    }
    make_food();                //生成第一个食物
    score = 0;                  //分数清零
    printDebugInfo();           //打印初始调试信息
    return;
}

void input() {
    if (_kbhit()) {             //检测是否有按键按下
        in_put = _getch();      //读取按键
        switch (in_put) {
        case 'a': if (dir == 3 || dir == 4)dir = 2; break; //a键：左
        case 'd': if (dir == 3 || dir == 4)dir = 1; break; //d键：右
        case 'w': if (dir == 1 || dir == 2)dir = 3; break; //w键：上
        case 's': if (dir == 1 || dir == 2)dir = 4; break; //s键：下
        }
    }
    return;
}

void move() {
    point tail = s.node[s.length]; //保存蛇尾坐标
    print(tail.x, tail.y, s.color, 0); //清空蛇尾
    mark[tail.x][tail.y] = 0;     //标记数组清空蛇尾
    for (int i = s.length; i >= 1; i--) { //蛇身节点后移
        s.node[i] = s.node[i - 1];
    }
    switch (dir) {              //蛇头移动
    case 1: s.node[0].x++; break; //右
    case 2: s.node[0].x--; break; //左
    case 3: s.node[0].y--; break; //上
    case 4: s.node[0].y++; break; //下
    }
    //撞边界判断（x=0，29 或 y=0，14 是边界）
    if (s.node[0].x == 0 || s.node[0].y == 0 || s.node[0].x == 29 || s.node[0].y == 14) {
        goto_xy(35, 10);
        set_console_color(12); //红色
        printf("【调试】撞边界死亡！蛇头坐标：X=%d,Y=%d\n", s.node[0].x, s.node[0].y);
        set_console_color(7);
        s.dead = 1;             //标记死亡
        return;
    }
    // 撞自身判断（标记数组=1表示有蛇身）
    if (mark[s.node[0].x][s.node[0].y] == 1) {
        goto_xy(35, 10);
        set_console_color(12);
        printf("【调试】撞自身死亡！蛇头坐标：X=%d,Y=%d\n", s.node[0].x, s.node[0].y);
        set_console_color(7);
        s.dead = 1;
        return;
    }
    mark[s.node[0].x][s.node[0].y] = 1; //标记蛇头新位置
    print(s.node[0].x, s.node[0].y, s.color, 1); // 绘制蛇头
    //吃食物判断（蛇头坐标=食物坐标）
    if (s.node[0].x == f.location.x && s.node[0].y == f.location.y) {
        goto_xy(35, 9);
        set_console_color(12);
        printf("【调试】吃到%s，分数+%d\n", foodTypeNames[f.type], (f.type == 1) ? 50 : 10);
        set_console_color(7);
        eat();                  //执行吃食物逻辑
    }
    Sleep(s.speed);             //控制移动速度
    return;
}

void die() {
    FILE* fp;                   //文件指针
    if ((fp = fopen("snake_h_score.txt", "r+")) == NULL) { // 打开最高分文件
        fp = fopen("snake_h_score.txt", "w+"); // 不存在则创建
    }
    fscanf(fp, "%d", &h_score); //读取历史最高分
    s.dead = 1;                 //标记死亡
    if (score > h_score) {      //刷新最高分
        fclose(fp);
        fp = fopen("snake_h_score.txt", "w+");
        fprintf(fp, "%d", score); //写入新最高分
        h_score = score;
    }
    system("CLS");              //清屏
    set_console_color(14);      //黄色
    printf("\n\n\n\n\n\t\t\t\t===== 游戏结束（调试信息）=====\n");
    set_console_color(7);
    printf("\t\t\t\t最终分数：%d\n", score); //打印最终分数
    printf("\t\t\t\t最终蛇长：%d\n", s.length); //打印最终长度
    printf("\t\t\t\t死亡时蛇头坐标：X=%d,Y=%d\n", s.node[0].x, s.node[0].y); //死亡坐标
    printf("\t\t\t\t=============================\n");
    printf("\n\n\t\t\t\t\t    你死了...\n\n");
    printf("\n\n\t\t\t\t\t当前分数为 : %d\n\n", score);
    printf("\t\t\t\t\t历史最高分数为 : %d\n\n", h_score);
    printf("\n\t\t\t\t  按 r键重来 , 按其余键结束游戏\n\n\n\n\n\n\n\n\n");
    char c = _getch();          //读取按键
    if (c == 'r') {             //r键：重来
        s.dead = 3;
    }
    else {                       //其他键：结束
        s.dead = 2;
    }
    fclose(fp);                 //关闭文件
    return;
}

void goto_xy(int x, int y)
{
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);//获取控制台句柄
    COORD pos = { x,y }; //坐标结构体
    SetConsoleCursorPosition(hOut, pos);//设置光标位置
}

void set_console_color(unsigned short color_index)// 设置字体颜色
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_index);
}

void HideCursor() //隐藏控制台光标）
{
    CONSOLE_CURSOR_INFO cursor; //光标信息结构体
    cursor.bVisible = FALSE;    //隐藏
    cursor.dwSize = sizeof(cursor); //光标大小
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); //获取标准输出句柄
    SetConsoleCursorInfo(handle, &cursor); // 应用光标设置
}
}

void make_boundary() {
    //绘制上下边界
    for (int i = 0; i <= 30; i++) {
        goto_xy(i, 0);   //光标移到
        printf("X");     //打印 X，形成上边界
        goto_xy(i, 15);  //光标移到
        printf("X");     //打印X形成下边界
    }

    //绘制左右边界
    for (int i = 0; i <= 15; i++) {
        goto_xy(0, i);   //光标移到 (0, i)
        printf("X");     //打印 X，形成左边界
        goto_xy(30, i);  //光标移到(30, i)
        printf("X");     //打印 X，形成右边界
    }
}
void init_snake() {                          //定义初始化蛇的函数，无返回值
    s.length = 5;                            //设置蛇的初始长度为5（实际绘制6节，长度变量记为5）
    s.dead = 0;                              //设置蛇的存活状态：0=存活（游戏开始时蛇是活的）
    s.speed = 100;                           //设置蛇的移动速度：100毫秒（值越小蛇移动越快）
    s.color = 7;                             //设置蛇的显示颜色：7=控制台默认浅灰色
    s.node[0].x = 6; s.node[0].y = 6;        //给蛇头（第0个节点）赋值坐标：横向x=6，纵向y=6
    mark[s.node[0].x][s.node[0].y] = 1;      //碰撞标记：蛇头位置设为1（表示有蛇身，防止撞自身/生成食物）
    goto_xy(6, 6);                           //把控制台光标移动到蛇头坐标(6,6)的位置
    printf("%c", getSkinChar(s.skin));       //打印蛇头：调用函数获取选中的数字皮肤（0/1/2/3）
    s.node[1].x = 5; s.node[1].y = 6;        //给第1节蛇身赋值坐标：x=5，y=6（蛇头左侧）
    mark[s.node[1].x][s.node[1].y] = 1;      //碰撞标记：第1节蛇身位置设为1
    goto_xy(5, 6);                           //光标移动到第1节蛇身坐标(5,6)
    printf("%c", getSkinChar(s.skin));       //打印第1节蛇身的数字皮肤
    s.node[2].x = 4; s.node[2].y = 6;        //给第2节蛇身赋值坐标：x=4，y=6
    mark[s.node[2].x][s.node[2].y] = 1;      //碰撞标记：第2节蛇身位置设为1
    goto_xy(4, 6);                           //光标移动到第2节蛇身坐标(4,6)
    printf("%c", getSkinChar(s.skin));       //打印第2节蛇身的数字皮肤
    s.node[3].x = 3; s.node[3].y = 6;        //给第3节蛇身赋值坐标：x=3，y=6
    mark[s.node[3].x][s.node[3].y] = 1;      //碰撞标记：第3节蛇身位置设为1
    goto_xy(3, 6);                           //光标移动到第3节蛇身坐标(3,6)
    printf("%c", getSkinChar(s.skin));       //打印第3节蛇身的数字皮肤
    s.node[4].x = 2; s.node[4].y = 6;        //给第4节蛇身赋值坐标：x=2，y=6
    mark[s.node[4].x][s.node[4].y] = 1;      //碰撞标记：第4节蛇身位置设为1
    goto_xy(2, 6);                           //光标移动到第4节蛇身坐标(2,6)
    printf("%c", getSkinChar(s.skin));       //打印第4节蛇身的数字皮肤
    s.node[5].x = 1; s.node[5].y = 6;        //给第5节蛇身（最后一节）赋值坐标：x=1，y=6
    mark[s.node[5].x][s.node[5].y] = 1;      //碰撞标记：最后一节蛇身位置设为1
    goto_xy(1, 6);                           //光标移动到最后一节蛇身坐标(1,6)
    printf("%c", getSkinChar(s.skin));       //打印最后一节蛇身的数字皮肤
    return;                                  //函数执行完毕，无返回值，回到调用处
}
void print(int x, int y, int color, int w) {
    HideCursor();               //隐藏光标
    set_console_color(color);   //设置颜色
    goto_xy(x, y);              //定位光标
    switch (w) {                //打印类型
    case 0: printf(" "); break; //空格（清空）
    case 1: printf("%c", getSkinChar(s.skin)); break; //蛇身蛇头
    case 2: printf("X"); break; //边界
    case 3: printf("♥"); break; //食物
    }
    set_console_color(7);       //恢复默认颜色
    return;
}
void make_food() {
    f.location.x = rand() % 28 + 1; //随机x坐标避开边界
    f.location.y = rand() % 13 + 1; //随机y坐标
    f.type = rand() % 4;        //随机食物类型
    int COLOR[4] = { 7 ,5, 2, 11 }; // 食物颜色：7=灰，5=紫，2=绿，11=浅蓝
    print(f.location.x, f.location.y, COLOR[f.type], 3); // 绘制食物（♥）
    goto_xy(35, 8);
    set_console_color(11);      //浅蓝
    printf("【调试】生成新食物：%s（X=%d,Y=%d）\n", foodTypeNames[f.type], f.location.x, f.location.y);
    set_console_color(7);
    return;
}

void eat() {
    int TYPE_SCORE[4] = { 10, 50, 10, 10 }; //食物分数：1=双倍长度食物+50分，其他+10分
    print(f.location.x, f.location.y, 7, 0); //清空食物位置（打印空格）
    mode = f.type;              // 缓存食物类型
    score += TYPE_SCORE[f.type]; //加分
    if (f.type == 1) {          //双倍长度食物
        s.length *= 2;
    }
    else {                      //其他食物（长度+1）
        s.length++;
    }
    make_food();                //生成新食物
    return;
}