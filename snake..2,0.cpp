/*************************************************************************
            > File Name: 贪吃蛇qwq          demo 2.0        update : 彻底消除闪动, 不再采用数组标记位置
            > Author: ewoifuoi
            > Update: VS2022适配+纯数字0123皮肤+调试信息打印
  ************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<windows.h>
#include<conio.h>

typedef enum {
    SKIN_0 = 0,
    SKIN_1 = 1,
    SKIN_2 = 2,
    SKIN_3 = 3
} SkinType;

const char skinChars[4] = { '0', '1', '2', '3' };
const char* skinNames[4] = { "0", "1", "2", "3" };

const char* foodTypeNames[4] = { "普通食物", "双倍长度食物", "基础食物", "加分食物" };
typedef struct Point {
    int x, y;
} point;
typedef struct Snake {
    int length;
    int speed;
    int color;
    int dead;
    point node[101];
    SkinType skin;
} snake;
typedef struct Food {
    point location;
    int type;
} food;

food f;
snake s;
int mark[30][15];
char in_put;
int dir;
int mode = 0;
int score;
int h_score;

void showSkinMenu();
char getSkinChar(SkinType type);
void printDebugInfo();
void HideCursor();
void goto_xy(int x, int y);
void set_console_color(unsigned short color_index);
void hello();
void init();
void input();
void move();
void die();
void make_boundary();
void init_snake();
void print(int x, int y, int color, int w);
void make_food();
void eat();

int main() {
    srand(time(0));
    showSkinMenu();
    hello();
    init();
    while (1) {
        input();
        move();
        printDebugInfo();
        if (s.dead == 1) {
            die();
            if (s.dead == 3) {
                system("CLS");
                showSkinMenu();
                hello();
                init();
                continue;
            }
            break;
        }
    }
    return 0;
}

void printDebugInfo() {
    goto_xy(35, 1);
    set_console_color(15);
    printf("===== 调试信息 =====\n");
    goto_xy(35, 2);
    printf("蛇头坐标：X=%d, Y=%d       \n", s.node[0].x, s.node[0].y);
    goto_xy(35, 3);
    printf("当前分数：%d              \n", score);
    goto_xy(35, 4);
    printf("蛇身长度：%d              \n", s.length);
    goto_xy(35, 5);
    printf("食物类型：%s             \n", foodTypeNames[f.type]);
    goto_xy(35, 6);
    printf("食物坐标：X=%d, Y=%d       \n", f.location.x, f.location.y);
    goto_xy(35, 7);
    const char* dirName = "";
    switch (dir) {
    case 1: dirName = "右"; break;
    case 2: dirName = "左"; break;
    case 3: dirName = "上"; break;
    case 4: dirName = "下"; break;
    }
    printf("移动方向：%s              \n", dirName);
    printf("====================\n");
    set_console_color(7);
}

void showSkinMenu() {
    system("CLS");
    HideCursor();
    int select = 0;
    while (1) {
        goto_xy(25, 5);
        set_console_color(14); 
        printf("===== 选择数字皮肤 =====\n");
        set_console_color(7);
        for (int i = 0; i < 4; i++) {
            goto_xy(30, 7 + i); 
            if (i == select) {
                set_console_color(10);
                printf("> %s\n", skinNames[i]);
            }
            else {
                set_console_color(7);
                printf("  %s\n", skinNames[i]);
            }
        }
        goto_xy(25, 12);
        set_console_color(8); 
        printf("↑↓键选择 | 回车键确认\n");
        set_console_color(7);
        if (_kbhit()) {
            in_put = _getch();
            switch (in_put) {
            case 72: select = (select - 1 + 4) % 4; break; 
            case 80: select = (select + 1) % 4; break; 
            case 13: s.skin = (SkinType)select; system("CLS"); return;
            }
        }
        Sleep(50);
    }
}

char getSkinChar(SkinType type) {
    return skinChars[type];
}

void hello() {
    printf("\n\n\n\n\n\t\t\t\t贪吃蛇");
    printf("qwq");
    set_console_color(8);
    printf("\tdemo 2.0");
    set_console_color(7);
    printf("\n\n\n\n\t\t\t\t  输入任意键开始游戏...\n\n\n\n\n\n\n\n\n");
    while (1) {
        if (_kbhit()) {
            system("CLS");
            return;
        }
        HideCursor();
        int qwq = rand() % 7 + 1;
        goto_xy(38, 5);
        printf("   ");
        goto_xy(38, 5);
        set_console_color(qwq);
        printf("qwq");
        set_console_color(7);
        goto_xy(34, 9);
        printf("输入任意键开始游戏...");
        Sleep(500);
        goto_xy(34, 9);
        printf("                     ");
        Sleep(200);
    }
    return;
}

void init() {
    make_boundary();
    dir = 1; 
    init_snake();
    memset(mark, 0, sizeof(mark));
    for (int i = 0; i <= s.length; i++) {
        mark[s.node[i].x][s.node[i].y] = 1;
    }
    make_food();
    score = 0;
    printDebugInfo();
    return;
}

void input() {
    if (_kbhit()) {
        in_put = _getch();
        switch (in_put) {
        case 'a': if (dir == 3 || dir == 4)dir = 2; break; 
        case 'd': if (dir == 3 || dir == 4)dir = 1; break; 
        case 'w': if (dir == 1 || dir == 2)dir = 3; break; 
        case 's': if (dir == 1 || dir == 2)dir = 4; break; 
        }
    }
    return;
}

void move() {
    point tail = s.node[s.length];
    print(tail.x, tail.y, s.color, 0);
    mark[tail.x][tail.y] = 0;
    for (int i = s.length; i >= 1; i--) {
        s.node[i] = s.node[i - 1];
    }
    switch (dir) {
    case 1: s.node[0].x++; break;
    case 2: s.node[0].x--; break;
    case 3: s.node[0].y--; break;
    case 4: s.node[0].y++; break;
    }
    if (s.node[0].x == 0 || s.node[0].y == 0 || s.node[0].x == 29 || s.node[0].y == 14) {
        goto_xy(35, 10);
        set_console_color(12);
        printf("【调试】撞边界死亡！蛇头坐标：X=%d,Y=%d\n", s.node[0].x, s.node[0].y);
        set_console_color(7);
        s.dead = 1;
        return;
    }
    if (mark[s.node[0].x][s.node[0].y] == 1) {
        goto_xy(35, 10);
        set_console_color(12);
        printf("【调试】撞自身死亡！蛇头坐标：X=%d,Y=%d\n", s.node[0].x, s.node[0].y);
        set_console_color(7);
        s.dead = 1;
        return;
    }
    mark[s.node[0].x][s.node[0].y] = 1;
    print(s.node[0].x, s.node[0].y, s.color, 1);
    if (s.node[0].x == f.location.x && s.node[0].y == f.location.y) {
        goto_xy(35, 9);
        set_console_color(12);
        printf("【调试】吃到%s，分数+%d\n", foodTypeNames[f.type],
            (f.type == 1) ? 50 : 10);
        set_console_color(7);
        eat();
    }
    Sleep(s.speed);
    return;
}

void die() {
    FILE* fp;
    if ((fp = fopen("snake_h_score.txt", "r+")) == NULL) {
        fp = fopen("snake_h_score.txt", "w+");
    }
    fscanf(fp, "%d", &h_score);
    s.dead = 1;
    if (score > h_score) {
        fclose(fp);
        fp = fopen("snake_h_score.txt", "w+");
        fprintf(fp, "%d", score);
        h_score = score;
    }
    system("CLS");
    set_console_color(14);
    printf("\n\n\n\n\n\t\t\t\t===== 游戏结束（调试信息）=====\n");
    set_console_color(7);
    printf("\t\t\t\t最终分数：%d\n", score);
    printf("\t\t\t\t最终蛇长：%d\n", s.length);
    printf("\t\t\t\t死亡时蛇头坐标：X=%d,Y=%d\n", s.node[0].x, s.node[0].y);
    printf("\t\t\t\t=============================\n");
    printf("\n\n\t\t\t\t\t    你死了...\n\n");
    printf("\n\n\t\t\t\t\t当前分数为 : %d\n\n", score);
    printf("\t\t\t\t\t历史最高分数为 : %d\n\n", h_score);
    printf("\n\t\t\t\t  按 r键重来 , 按其余键结束游戏\n\n\n\n\n\n\n\n\n");
    char c = _getch();
    if (c == 'r') {
        s.dead = 3; 
    }
    else {
        s.dead = 2; 
    }
    fclose(fp);
    return;
}

void goto_xy(int x, int y)
{
    HANDLE hOut;
    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x,y };
    SetConsoleCursorPosition(hOut, pos);
}

void set_console_color(unsigned short color_index)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color_index);
}

void HideCursor()
{
    CONSOLE_CURSOR_INFO cursor;
    cursor.bVisible = FALSE;
    cursor.dwSize = sizeof(cursor);
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorInfo(handle, &cursor);
}

void make_boundary() {
    for (int i = 0; i <= 30; i++) {
        goto_xy(i, 0);
        printf("X");
        goto_xy(i, 15);
        printf("X");
    }
    for (int i = 0; i <= 15; i++) {
        goto_xy(0, i);
        printf("X");
        goto_xy(30, i);
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
    mark[s.node[0].x][s.node[0].y] = 1;
    goto_xy(6, 6);
    printf("%c", getSkinChar(s.skin));
    s.node[1].x = 5; s.node[1].y = 6;
    mark[s.node[1].x][s.node[1].y] = 1;
    goto_xy(5, 6);
    printf("%c", getSkinChar(s.skin));
    s.node[2].x = 4; s.node[2].y = 6;
    mark[s.node[2].x][s.node[2].y] = 1;
    goto_xy(4, 6);
    printf("%c", getSkinChar(s.skin));
    s.node[3].x = 3; s.node[3].y = 6;
    mark[s.node[3].x][s.node[3].y] = 1;
    goto_xy(3, 6);
    printf("%c", getSkinChar(s.skin));
    s.node[4].x = 2; s.node[4].y = 6;
    mark[s.node[4].x][s.node[4].y] = 1;
    goto_xy(2, 6);
    printf("%c", getSkinChar(s.skin));
    s.node[5].x = 1; s.node[5].y = 6;
    mark[s.node[5].x][s.node[5].y] = 1;
    goto_xy(1, 6);
    printf("%c", getSkinChar(s.skin));
    return;
}

void print(int x, int y, int color, int w) {
    HideCursor();
    set_console_color(color);
    goto_xy(x, y);
    switch (w) {
    case 0: printf(" "); break;
    case 1: printf("%c", getSkinChar(s.skin)); break;
    case 2: printf("X"); break;
    case 3: printf("♥"); break;
    }
    set_console_color(7);
    return;
}

void make_food() {
    f.location.x = rand() % 28 + 1;
    f.location.y = rand() % 13 + 1;
    f.type = rand() % 4;
    int COLOR[4] = { 7 ,5, 2, 11 };
    print(f.location.x, f.location.y, COLOR[f.type], 3);
    goto_xy(35, 8);
    set_console_color(11);
    printf("【调试】生成新食物：%s（X=%d,Y=%d）\n", foodTypeNames[f.type], f.location.x, f.location.y);
    set_console_color(7);
    return;
}

void eat() {
    int TYPE_SCORE[4] = { 10, 50, 10, 10 };
    print(f.location.x, f.location.y, 7, 0);
    mode = f.type;
    score += TYPE_SCORE[f.type];
    if (f.type == 1) {
        s.length *= 2;
    }
    else {
        s.length++;
    }
    make_food(); 
    return;
}