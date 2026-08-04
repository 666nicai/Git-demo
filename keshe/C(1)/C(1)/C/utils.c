// ============================================================
// 文件名: utils.c
// 描述: 通用工具函数实现（最终完美版）
// ============================================================

#include "his.h"
#include <windows.h>
#include <conio.h>

void clearScreen() {
    system("cls");
    setColor(COLOR_DEFAULT);
}

void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void showCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// 绘制双线边框（纯 ASCII，带阴影）
void drawModernBox(int x, int y, int w, int h, const char* title) {
    int i;
    // 阴影
    setColor(0x08);
    for (i = 1; i <= h; i++) {
        gotoxy(x + w, y + i);
        putchar(' ');
    }
    gotoxy(x + 1, y + h);
    for (i = 0; i < w; i++) putchar(' ');
    
    // 主边框
    setColor(COLOR_BORDER);
    gotoxy(x, y);
    putchar('+');
    for (i = 0; i < w - 2; i++) putchar('-');
    putchar('+');

    for (i = 1; i < h - 1; i++) {
        gotoxy(x, y + i);
        putchar('|');
        gotoxy(x + w - 1, y + i);
        putchar('|');
    }

    gotoxy(x, y + h - 1);
    putchar('+');
    for (i = 0; i < w - 2; i++) putchar('-');
    putchar('+');

    if (title) {
        int len = strlen(title);
        setColor(COLOR_TITLE);
        gotoxy(x + (w - len) / 2, y);
        printf(" %s ", title);
    }
    setColor(COLOR_DEFAULT);
}

// 安全输入
void safeInput(char* buffer, int maxLen, int x, int y) {
    int i = 0;
    char ch;
    gotoxy(x, y);
    setColor(COLOR_INPUT_BG);
    for (int j = 0; j < maxLen; j++) putchar(' ');
    gotoxy(x, y);
    setColor(COLOR_DEFAULT);
    showCursor();
    while (1) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') {
            buffer[i] = '\0';
            break;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                gotoxy(x + i, y);
                putchar(' ');
                gotoxy(x + i, y);
            }
        } else if (i < maxLen - 1 && ch >= 32 && ch <= 126) {
            buffer[i++] = ch;
            putchar(ch);
        }
    }
    hideCursor();
    setColor(COLOR_DEFAULT);
}

// 密码输入
int getPassword(char* pass, int maxLen) {
    int i = 0;
    char ch;
    showCursor();
    while (1) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') {
            pass[i] = '\0';
            break;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < maxLen - 1) {
            pass[i++] = ch;
            putchar('*');
        }
    }
    hideCursor();
    return i;
}

// 打印表头
void printTableHeader(const char* headers[], int widths[], int count) {
    setColor(COLOR_HEADER);
    for (int i = 0; i < count; i++) printf("%-*s", widths[i], headers[i]);
    printf("\n");
    setColor(COLOR_DEFAULT);
}

// 打印表格行
void printTableRow(const char* values[], int widths[], int count, int rowColor) {
    setColor(rowColor);
    for (int i = 0; i < count; i++) printf("%-*s", widths[i], values[i]);
    printf("\n");
    setColor(COLOR_DEFAULT);
}

// 底部提示
void pressAnyKey() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    gotoxy(0, rows - 1);
    setColor(COLOR_WARNING);
    printf("  >>> 按任意键继续 <<<");
    setColor(COLOR_DEFAULT);
    _getch();
}

char* getCurrentTime() {
    static char buffer[30];
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

// ========== 精致标题（优雅大气，高度可控） ==========
void printTitle() {
    // 第 1 行：顶部空行
    printf("\n");
    
    // 第 2 行：顶部双线装饰
    setColor(0x08);  // 深灰色
    printf("  +===============================================================================+\n");
    
    // 第 3 行：医院名称（大号，金色）
    setColor(0x0E);  // 亮黄色
    printf("  |"); 
    setColor(0x0E);
    printf("                                  K K 医 院                                    ");
    setColor(0x0E);
    printf("|\n");
    
    // 第 4 行：英文名称
    setColor(0x08);
    printf("  |");
    setColor(0x0B);  // 亮青色
    printf("                               Zmjjkk HOSPITAL                                 ");
    setColor(0x08);
    printf("|\n");
    
    // 第 5 行：分隔线
    setColor(0x0C);
    printf("  |                          都是铜陵人  我原本没想酱味大鸡                       |\n");
    
    // 第 6 行：系统名称
    setColor(0x08);
    printf("  |");
    setColor(0x0F);  // 亮白色
    printf("                          医 院 信 息 管 理 系 统  (HIS)                       ");
    setColor(0x08);
    printf("|\n");
    
    // 第 7 行：版本号
    setColor(0x08);
    printf("  |");
    setColor(0x08);
    printf("                                 Version 2.0                                   ");
    setColor(0x08);
    printf("|\n");
    
    // 第 8 行：底部双线装饰
    setColor(0x08);
    printf("  +===============================================================================+\n");
    
    // 第 9 行：额外空行，让标题与后续内容呼吸
    printf("\n");
    
    setColor(COLOR_DEFAULT);  // 恢复默认颜色
}