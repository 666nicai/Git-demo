// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：MicroAccountWRH.cpp
#include "MicroAccountWRH.h"
#include "UserWRH.h"
#include <iostream>

// =====================================================================
// MicroAccountWRH.cpp  微X账号抽象基类实现
// =====================================================================

MicroAccountWRH::MicroAccountWRH(UserWRH* owner, MicroType t, const MyDateWRH& apply)
    : owner_(owner), type_(t), logged_(false), applyDate_(apply) {}

MicroAccountWRH::~MicroAccountWRH() {}

// 在该服务下添加好友（互为好友，对方需开通同一服务）
bool MicroAccountWRH::addFriend(UserWRH* who, const std::string& remark) {
    if (!who || who == owner_) return false;
    // 对方必须也开通了本服务，否则不能成为该服务下的好友
    MicroAccountWRH* other = who->account(type_);
    if (!other) {
        std::cout << "    [提示] " << who->nick() << " 尚未开通 " << svcName()
                  << "，无法成为 " << svcName() << " 好友。\n";
        return false;
    }
    if (friends_.has(who)) {
        std::cout << "    [提示] " << who->nick() << " 已经是你的 " << svcName() << " 好友。\n";
        return false;
    }
    // 自己这侧 + 对方那侧（互为好友）
    friends_.add(who, remark);
    other->friends().add(owner_, std::string());   // 对方列表：备注留空，显示对方昵称
    return true;
}

// 删除好友（双向解除）
bool MicroAccountWRH::delFriend(UserWRH* who) {
    if (!who) return false;
    friends_.remove(who);
    MicroAccountWRH* other = who->account(type_);
    if (other) other->friends().remove(owner_);
    return true;
}

// 公共展示：服务类型/号码/ID策略/T龄/登录态/好友
void MicroAccountWRH::show() const {
    std::cout << "  [" << svcName() << "] 用户：" << owner_->nick()
              << "  号码：" << svcNo() << "\n";
    std::cout << "    |-- ID策略：";
    if (isSharedID())
        std::cout << svcName() << "与另一微X服务共享主号（同一ID）\n";
    else if (isWeChat())
        std::cout << "微信为独立ID（微信号），可与QQ绑定\n";
    else
        std::cout << "独立ID\n";

    std::cout << "    |-- T龄：" << tAge() << " 天（本号申请时间 "
              << applyDate_.str() << "）\n";
    std::cout << "    |-- 登录状态：" << (logged_ ? "已登录" : "未登录") << "\n";
    std::cout << "    |-- 好友数：" << friends_.count() << "\n";
    friends_.list();
}
