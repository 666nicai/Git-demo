// ============================================================
// 文件名: UserWRH.cpp
// 描述  : 平台用户实现——开通/注销、登录联动、微信绑定、资料展示
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#include "UserWRH.h"
#include "ShortTypes.h"
#include "MicroAccountWRH.h"
#include "QQAccountWRH.h"
#include "WeChatAccountWRH.h"
#include "WeiboAccountWRH.h"
#include <bits/stdc++.h>

using namespace std;

UserWRH::UserWRH(const string& id, const string& nick,
                 const Date& birth, const Date& apply,
                 const string& city)
    : BaseInfo(nick, birth, apply, city), mainNo_(id) {}

UserWRH::~UserWRH() {
    for (size_t i = 0; i < accs_.size(); ++i) delete accs_[i];
    accs_.clear();
}

set<MicroType> User::openedSet() const {
    set<MicroType> s;
    for (size_t i = 0; i < accs_.size(); ++i)
        s.insert(accs_[i]->type());
    return s;
}

bool User::hasService(MicroType t) const {
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->type() == t) return true;
    return false;
}

Account* User::account(MicroType t) const {
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->type() == t) return accs_[i];
    return nullptr;
}

// 开通一个服务。QQ/微博用共享主号；微信第一次开通时分配独立号。
Account* User::openService(MicroType t, bool quiet) {
    if (hasService(t)) {
        if (!quiet)
            cout << "    [提示] " << nick() << " 已开通 "
                 << microName(t) << "，无需重复开通。\n";
        return account(t);
    }

    Account* p = nullptr;
    if (t == MQQ) {
        p = new QQAccount(this, applyDate());
    } else if (t == MWB) {
        p = new WeiboAccount(this, applyDate());
    } else if (t == MWX) {
        if (wxNo_.empty()) wxNo_ = "wx" + mainNo_;   // 微信独立号：wx + 主号
        WeChatAccount* w = new WeChatAccount(this, applyDate());
        w->setWxNo(wxNo_);
        p = w;
    }

    if (p) {
        accs_.push_back(p);
        if (!quiet)
            cout << "    [开通] " << nick() << " 开通 " << microName(t)
                 << " 成功，号码：" << p->svcNo() << "\n";
    }
    return p;
}

// 注销服务：除了删自己的账号，还要把“别人好友列表里的我”一并删掉，
// 否则对方会留下一个点不开的单向好友，数据就不一致了。
void User::closeService(MicroType t) {
    for (size_t i = 0; i < accs_.size(); ++i) {
        if (accs_[i]->type() != t) continue;

        Account* acc = accs_[i];
        const vector<FriendInfo>& fs = acc->friends().all();
        int removed = 0;
        for (size_t j = 0; j < fs.size(); ++j) {
            Account* other = fs[j].person()->account(t);
            if (!other) continue;
            other->friends().remove(this);
            ++removed;
        }

        delete acc;
        accs_.erase(accs_.begin() + i);
        cout << "    [注销] " << nick() << " 已注销 " << microName(t)
             << " 服务（同时从 " << removed << " 位好友的列表中移除）。\n";
        return;
    }
    cout << "    [提示] 未开通 " << microName(t) << "，无法注销。\n";
}

// 一个服务登录 → 该用户所有已开通服务全部登录
bool User::login(MicroType t) {
    if (!account(t)) {
        cout << "    [提示] " << nick() << " 未开通 "
             << microName(t) << "，无法登录。\n";
        return false;
    }

    for (size_t i = 0; i < accs_.size(); ++i)
        accs_[i]->setLogged(true);

    cout << "    [登录] " << nick() << " 在 [" << microName(t)
         << "] 登录，已开通的 ";
    for (size_t i = 0; i < accs_.size(); ++i)
        cout << (i ? "、" : "") << microName(accs_[i]->type());
    cout << " 全部自动登录。\n";
    return true;
}

void User::logoutAll() {
    for (size_t i = 0; i < accs_.size(); ++i)
        accs_[i]->setLogged(false);
}

bool User::anyLogged() const {
    for (size_t i = 0; i < accs_.size(); ++i)
        if (accs_[i]->logged()) return true;
    return false;
}

WeChatAccount* User::wxAccount() const {
    return dynamic_cast<WeChatAccount*>(account(MWX));
}

void User::bindWxToQQ(const string& qq, bool quiet) {
    WeChatAccount* w = wxAccount();
    if (!w) {
        if (!quiet) cout << "    [提示] 未开通微信，无法绑定。\n";
        return;
    }
    w->bindQQ(qq);
    if (!quiet)
        cout << "    [绑定] 微信号 " << w->wxNo()
             << " 已绑定QQ " << qq << "\n";
}

// 基本资料：复用 BaseInfo::show()，再补主号、微信号、开通状态
void User::showProfile() const {
    cout << "  ---------- 用户资料 ----------\n";
    BaseInfo::show();
    cout << "    |-- 主号(QQ/微博共享)： " << mainNo_ << "\n";
    if (!wxNo_.empty())
        cout << "    |-- 微信号(微信独立ID)：" << wxNo_ << "\n";

    cout << "    |-- 已开通服务(" << openedCount() << ")：";
    if (accs_.empty()) cout << "（无）";
    for (size_t i = 0; i < accs_.size(); ++i)
        cout << (i ? "、" : "") << microName(accs_[i]->type())
             << (accs_[i]->logged() ? "[在线]" : "[离线]");
    cout << "\n";
}

// 每个账号的详情：这里通过基类指针调用 show()，运行时会走到各自的版本
void User::showOpened() const {
    if (accs_.empty()) {
        cout << "    （未开通任何服务）\n";
        return;
    }
    for (size_t i = 0; i < accs_.size(); ++i)
        accs_[i]->show();
}
