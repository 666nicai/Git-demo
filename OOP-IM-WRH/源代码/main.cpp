// ============================================================
// 文件名: main.cpp
// 描述  : 程序入口——设置 UTF-8 控制台、显示标题、启动平台
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#include "PlatformWRH.h"
#include "ShortTypes.h"
#include <bits/stdc++.h>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

int main() {
    // Windows 控制台默认不是 UTF-8，先切一下，中文才不会乱码
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    cout << "\n"
            "  ==================================================\n"
            "  面向对象程序设计课程设计 —— 模拟即时通信系统\n"
            "  腾讯立体社交平台(QQ / 微博 / 微信)\n"
            "  班级：软件一班(552501班)  学号：55250124  姓名：魏荣赫\n"
            "  类名统一带姓名缩写 WRH（5个技术层次均实现）\n"
            "  ==================================================\n";

    Platform platform("platform_data.txt");   // 构造时自动读文件或建演示数据
    platform.run();                              // 数字化主菜单
    cout << "程序结束，谢谢使用。\n";
    return 0;
}