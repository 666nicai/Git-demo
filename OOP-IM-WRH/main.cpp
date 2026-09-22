#include "PlatformWRH.h"
#include <iostream>

// 工程：模拟即时通信系统（腾讯立体社交平台）—— 入口
// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
    // 在 Windows 下让控制台按 UTF-8 显示中文
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif

    std::cout << "\n"
                 "  ==================================================\n"
                 "  面向对象程序设计课程设计 —— 模拟即时通信系统\n"
                 "  腾讯立体社交平台(QQ / 微博 / 微信)\n"
                 "  班级：软件一班(552401班)  学号：55250124  姓名：魏荣赫\n"
                 "  类名统一带姓名缩写 WRH（5个技术层次均实现）\n"
                 "  ==================================================\n";

    PlatformWRH platform("platform_data.txt");   // 构造时自动实例化数据
    platform.run();                              // 数字化主菜单
    std::cout << "程序结束，谢谢使用。\n";
    return 0;
}
