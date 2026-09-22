// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：UserWRH.cpp
#include "UserWRH.h"
#include "MicroAccountWRH.h"
#include "QQAccountWRH.h"
#include "WeiboAccountWRH.h"
#include "WeChatAccountWRH.h"
#include <iostream>

UserWRH::UserWRH(const std::string& id, const std::string& nick,
                 const MyDateWRH& birth, const MyDateWRH& apply,
                 const std::string& city)
    : BaseInfoWRH(nick, birth, apply, city), mainNo_(id) {}

UserWRH::~UserWRH() {
    for (size_t i = 0; i < accs_.size(); ++i) delete accs_[i];
    accs_.clear();
}

std::set<MicroType> UserWRH::openedSet() const {
    std::set<MicroType> s;
    for (size_t i = 0; i < accs_.size(); ++i) s.insert(accs_[i]->type());
    return s;
}

bool UserWRH::hasService(MicroType t) const {
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->type() == t) return true;
    return false;
}

MicroAccountWRH* UserWRH::account(MicroType t) const {
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->type() == t) return accs_[i];
    return nullptr;
}

// 开通一个微X服务（可开通N个）。QQ/微博共享主号，微信分配独立号。
MicroAccountWRH* UserWRH::openService(MicroType t) {
    if (hasService(t)) {
        std::cout << "    [提示] " << nick() << " 已开通 " << microName(t) << "，无需重复开通。\n";
        return account(t);
    }
    MicroAccountWRH* p = nullptr;
    if (t == MQQ) {
        p = new QQAccountWRH(this, applyDate());       // 共享主号 mainNo_
    } else if (t == MWB) {
        p = new WeiboAccountWRH(this, applyDate());    // 共享主号 mainNo_
    } else if (t == MWX) {
        if (wxNo_.empty())
            wxNo_ = "wx" + mainNo_;   // 微信独立ID：由平台自动分配(与QQ号不同位)
        WeChatAccountWRH* w = new WeChatAccountWRH(this, applyDate());
        w->setWxNo(wxNo_);
        p = w;
    }
    if (p) {
        accs_.push_back(p);
        std::cout << "    [开通] " << nick() << " 开通 " << microName(t)
                  << " 成功，号码：" << p->svcNo() << "\n";
    }
    return p;
}

void UserWRH::closeService(MicroType t) {
    for (size_t i = 0; i < accs_.size(); ++i) {
        if (accs_[i]->type() == t) {
            delete accs_[i];
            accs_.erase(accs_.begin() + i);
            std::cout << "    [注销] " << nick() << " 已注销 " << microName(t) << " 服务。\n";
            return;
        }
    }
    std::cout << "    [提示] 未开通 " << microName(t) << "，无法注销。\n";
}

// 一个服务登录 → 其它已开通服务自动登录
bool UserWRH::login(MicroType t) {
    MicroAccountWRH* one = account(t);
    if (!one) {
        std::cout << "    [提示] " << nick() << " 未开通 " << microName(t) << "，无法登录。\n";
        return false;
    }
    for (size_t i = 0; i < accs_.size(); ++i) {
        accs_[i]->setLogged(true);   // 全部已开通服务自动登录
    }
    std::cout << "    [登录] " << nick() << " 在 [" << microName(t) << "] 登录，"
              << "其开通的其它服务(";
    bool first = true;
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->type() != t) {
            if (!first) std::cout << "、";
            first = false;
            std::cout << microName(accs_[i]->type());
        }
    std::cout << ") 已自动登录。\n";
    return true;
}

void UserWRH::logoutAll() {
    for (size_t i = 0; i < accs_.size(); ++i) accs_[i]->setLogged(false);
}

bool UserWRH::anyLogged() const {
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->logged()) return true;
    return false;
}

WeChatAccountWRH* UserWRH::wxAccount() const {
    return dynamic_cast<WeChatAccountWRH*>(account(MWX));
}

void UserWRH::bindWxToQQ(const std::string& qq) {
    WeChatAccountWRH* w = wxAccount();
    if (!w) { std::cout << "    [提示] 未开通微信，无法绑定。\n"; return; }
    w->bindQQ(qq);
    std::cout << "    [绑定] 微信号 " << w->wxNo() << " 已绑定QQ " << qq << "\n";
}

// 基本信息展示（复用抽象基础类的 show 成员）
void UserWRH::showProfile() const {
    std::cout << "  ---------- 用户资料 ----------\n";
    BaseInfoWRH::show();                     // 昵称/出生/所在地/T龄
    std::cout << "    |-- 主号(QQ/微博共享)： " << mainNo_ << "\n";
    if (!wxNo_.empty())
        std::cout << "    |-- 微信号(微信独立ID)：" << wxNo_ << "\n";
    std::cout << "    |-- 已开通服务(" << openedCount() << ")：";
    if (accs_.empty()) std::cout << "（无）";
    for (size_t i = 0; i < accs_.size(); ++i) {
        if (i) std::cout << "、";
        std::cout << microName(accs_[i]->type()) << (accs_[i]->logged() ? "[在线]" : "[离线]");
    }
    std::cout << "\n";
}

// 每个开通服务账号的详情（运行时多态：QQ/微博/微信各自的展示）
void UserWRH::showOpened() const {
    if (accs_.empty()) { std::cout << "    （未开通任何服务）\n"; return; }
    for (size_t i = 0; i < accs_.size(); ++i)
        accs_[i]->show();
}
