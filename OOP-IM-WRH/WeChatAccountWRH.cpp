// ============================================================
// 文件名: WeChatAccountWRH.cpp
// 描述  : 微信账号实现——独立微信号与QQ绑定展示
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "WeChatAccountWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"
#include <iostream>

using namespace std;

WeChatAccountWRH::WeChatAccountWRH(User* owner, const Date& apply)
    : Account(owner, MWX, apply) {}

// 微信独立ID：号码就是自己的微信号
string WeChatAccount::svcNo() const {
    return wxNo_;
}

// 先按公共账号格式显示，再补一行微信特有的绑定状态
void WeChatAccount::show() const {
    Account::show();
    cout << "    |-- 微信采用独立ID，微信号：" << wxNo_;
    if (hasBoundQQ())
        cout << "（已绑定QQ：" << boundQQ_ << "）";
    else
        cout << "（未绑定QQ，可绑定）";
    cout << "\n";
}
