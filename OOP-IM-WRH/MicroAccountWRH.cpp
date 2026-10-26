// ============================================================
// 文件名: MicroAccountWRH.cpp
// 描述  : 微X账号抽象基类实现——好友关系与公共展示
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "MicroAccountWRH.h"
#include "UserWRH.h"
#include <iostream>

using namespace std;

MicroAccountWRH::MicroAccountWRH(UserWRH* owner, MicroType t,
                                 const MyDateWRH& apply)
    : owner_(owner), type_(t), logged_(false), applyDate_(apply) {}

MicroAccountWRH::~MicroAccountWRH() {}

// 在本服务下加好友：自己这侧写备注，对方那侧也加一条（备注留空）
bool MicroAccountWRH::addFriend(UserWRH* who, const string& remark) {
    if (!who || who == owner_) return false;

    // 对方必须也开通了本服务，否则这个服务下根本没有他的号
    MicroAccountWRH* other = who->account(type_);
    if (!other) {
        cout << "    [提示] " << who->nick() << " 尚未开通 " << svcName()
             << "，无法成为 " << svcName() << " 好友。\n";
        return false;
    }
    if (friends_.has(who)) {
        cout << "    [提示] " << who->nick() << " 已经是你的 "
             << svcName() << " 好友。\n";
        return false;
    }

    friends_.add(who, remark);                    // 我这边：带备注
    other->friends().add(owner_, string());       // 对方那边：备注先留空
    return true;
}

// 删好友要双向解除，不然对方列表里会留下“单向好友”
bool MicroAccountWRH::delFriend(UserWRH* who) {
    if (!who) return false;
    bool removed = friends_.remove(who);
    MicroAccountWRH* other = who->account(type_);
    if (other) other->friends().remove(owner_);
    return removed;
}

// 公共展示：服务、号码、ID策略、T龄、登录状态、好友
void MicroAccountWRH::show() const {
    cout << "  [" << svcName() << "] 用户：" << owner_->nick()
         << "  号码：" << svcNo() << "\n";

    cout << "    |-- ID策略：";
    if (isSharedID())
        cout << svcName() << "与另一微X服务共享主号（同一ID）\n";
    else if (isWeChat())
        cout << "微信为独立ID（微信号），可与QQ绑定\n";
    else
        cout << "独立ID\n";

    cout << "    |-- T龄：" << tAge() << " 天（本号申请时间 "
         << applyDate_.str() << "）\n";
    cout << "    |-- 登录状态：" << (logged_ ? "已登录" : "未登录") << "\n";
    cout << "    |-- 好友数：" << friends_.count() << "\n";
    friends_.list();
}
