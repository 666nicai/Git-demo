// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：WeChatAccountWRH.cpp
#include "WeChatAccountWRH.h"
#include "UserWRH.h"
#include <iostream>

WeChatAccountWRH::WeChatAccountWRH(UserWRH* owner, const MyDateWRH& apply)
    : MicroAccountWRH(owner, MWX, apply) {}

// 微信独立ID：号码 = 该用户独立的微信号
std::string WeChatAccountWRH::svcNo() const {
    return wxNo_;
}

// 展示微信账号：独立号 + 与QQ绑定状态
void WeChatAccountWRH::show() const {
    MicroAccountWRH::show();   // 先按公共账号格式展示
    std::cout << "    |-- 微信采用独立ID，微信号：" << wxNo_;
    if (hasBoundQQ())
        std::cout << "（已绑定QQ：" << boundQQ_ << "）";
    else
        std::cout << "（未绑定QQ，可绑定）";
    std::cout << "\n";
}
