// ============================================================
// 文件名: utils.c
// 描述: 通用工具函数实现（最终完美版）
// ============================================================

#include "his.h"
#include <windows.h>
#include <conio.h>

void clearScreen() {
    system("cls"); // 系统命令清屏
    setColor(COLOR_DEFAULT);
}

void setColor(int color) { // 设置控制台输出文字的颜色
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void gotoxy(int x, int y) { // 将控制台光标移动到指定坐标位置
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCursor() { // 隐藏控制台光标
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = FALSE; // 设置光标不可见
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void showCursor() { // 显示控制台光标
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
        gotoxy(x + w, y + i); // 移动光标到阴影位置
        putchar(' ');         // 输出空格作为阴影
    }
    gotoxy(x + 1, y + h);
    for (i = 0; i < w; i++) putchar(' ');
    
    // 主边框
    setColor(COLOR_BORDER);
    gotoxy(x, y);                 // 绘制左上角顶点
    putchar('+');
    for (i = 0; i < w - 2; i++) putchar('-');
    putchar('+');

    for (i = 1; i < h - 1; i++) { // 绘制左右竖线（边框主体）
        gotoxy(x, y + i);
        putchar('|');
        gotoxy(x + w - 1, y + i);
        putchar('|');
    }

    gotoxy(x, y + h - 1);         // 绘制左下角顶点
    putchar('+');
    for (i = 0; i < w - 2; i++) putchar('-');
    putchar('+');

    if (title) {                  // 绘制标题
        int len = strlen(title);
        setColor(COLOR_TITLE);
        gotoxy(x + (w - len) / 2, y);
        printf(" %s ", title);
    }
    setColor(COLOR_DEFAULT);
}

// 安全输入（限定长度、带背景填充、防输入越界）
void safeInput(char* buffer, int maxLen, int x, int y) {
    int i = 0;
    int ch;                           // 改为 int，容纳 _getch() 返回值
    gotoxy(x, y);
    setColor(COLOR_INPUT_BG);
    for (int j = 0; j < maxLen; j++) putchar(' '); 
    gotoxy(x, y);
    setColor(COLOR_DEFAULT);
    showCursor();
    while (1) {
        ch = _getch();                // 无回显读取
        if (ch == '\r' || ch == '\n') {
            buffer[i] = '\0';
            break;
        } else if (ch == '\b') {     // 退格键
            if (i > 0) {
                // 判断上一个字符是否为双字节汉字
                int isDouble = 0;
                if (i >= 2) {
                    unsigned char first  = (unsigned char)buffer[i-2];
                    unsigned char second = (unsigned char)buffer[i-1];
                    if (first >= 0x81 && first <= 0xFE &&
                        second >= 0x40 && second <= 0xFE) {
                        isDouble = 1;
                    }
                }
                if (isDouble) {
                    i -= 2;
                    gotoxy(x + i, y);
                    putchar(' '); putchar(' ');   // 擦除两个字符
                    gotoxy(x + i, y);
                } else {
                    i--;
                    gotoxy(x + i, y);
                    putchar(' ');
                    gotoxy(x + i, y);
                }
            }
        } else if (ch >= 32 && ch <= 126) {     // 普通 ASCII
            if (i < maxLen - 1) {
                buffer[i++] = (char)ch;
                putchar((char)ch);
            }
        } else if ((unsigned char)ch >= 0x81 &&
                   (unsigned char)ch <= 0xFE) { // 汉字第一字节
            if (i < maxLen - 2) {
                int ch2 = _getch();             // 读取第二字节
                if (ch2 != -1 &&
                    (unsigned char)ch2 >= 0x40 &&
                    (unsigned char)ch2 <= 0xFE) {
                    buffer[i++] = (char)ch;
                    buffer[i++] = (char)ch2;
                    putchar((char)ch);
                    putchar((char)ch2);
                }
                // 第二字节非法则忽略，不写入
            }
        }
        // 其他控制字符（如 Esc、方向键等）忽略
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
        if (ch == '\r' || ch == '\n') { // 回车/换行：结束输入
            pass[i] = '\0';
            break;
        } else if (ch == '\b') {        // 退格键：删除已输入字符
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < maxLen - 1) {    // 未到最大长度：保存字符并显示*
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
    for (int i = 0; i < count; i++) printf("%-*s", widths[i], headers[i]);// 循环打印每列表头
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

// 底部提示   ？？？
void pressAnyKey() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;                    // 存储控制台屏幕缓冲区信息
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // 获取标准输出句柄
    GetConsoleScreenBufferInfo(hConsole, &csbi);        // 获取屏幕缓冲区信息
    int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;// 计算控制台可见行数
    gotoxy(0, rows - 1);                                // 将光标移动到最后一行
    setColor(COLOR_WARNING);
    printf("  >>> 按任意键继续 <<<");
    setColor(COLOR_DEFAULT);
    _getch();
}

// 获取当前系统时间
char* getCurrentTime() {
    static char buffer[30];
    time_t rawtime;       // 存储原始时间
    struct tm* timeinfo;  // 存储分解后的时间结构体
    time(&rawtime);       // 获取当前时间
    timeinfo = localtime(&rawtime);// 将时间转换为本地时间结构体
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo); // 格式化时间到缓冲区（年-月-日 时:分:秒）
    return buffer;        // 返回时间字符串指针
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