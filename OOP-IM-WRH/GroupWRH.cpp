// ============================================================
// 文件名: GroupWRH.cpp
// 描述  : 群抽象基类实现——公共入群/踢人/管理员/退群流程
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "GroupWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"
#include <iostream>

using namespace std;

GroupWRH::GroupWRH(int gno, const string& name, MicroType svc, User* owner)
    : gno_(gno), name_(name), svc_(svc) {
    // 建群的人自动成为群主，并进入成员容器
    if (owner) members_.add(owner, ROLE_OWNER);
}

User* Group::owner() const {
    const vector<GroupMember>& v = members_.all();
    for (size_t i = 0; i < v.size(); ++i)
        if (v[i].role() == ROLE_OWNER) return v[i].user();
    return nullptr;
}

// 入群：QQ群可以主动申请；微信群只能在 canApplyJoin() 里返回 false，
// 然后由 requestJoin() 打印“只能邀请/推荐”的提示。
bool Group::join(User* who) {
    return requestJoin(who);
}

bool Group::requestJoin(User* who) {
    if (!who) return false;
    if (members_.isMember(who)) {
        cout << "    [提示] " << who->nick() << " 已在本群。\n";
        return false;
    }
    if (!who->account(svc_)) {
        cout << "    [提示] " << who->nick() << " 未开通 "
             << microName(svc_) << "，不能加入该" << kindName() << "。\n";
        return false;
    }
    if (!canApplyJoin()) {
        cout << "    [拒绝] " << kindName()
             << "不支持主动申请加入，只能由 " << microName(svc_)
             << " 成员推荐/邀请。\n";
        return false;
    }

    members_.add(who, ROLE_MEMBER);
    cout << "    [入群] " << who->nick() << " 申请成功，已加入["
         << name_ << "]。\n";
    return true;
}

// 邀请/推荐：两种群都允许，微信群就是靠这条路径加人的
bool Group::inviteJoin(User* actor, User* who) {
    if (!actor || !who) return false;
    if (!members_.isMember(actor)) {
        cout << "    [提示] " << actor->nick() << " 不是本群成员，无权邀请。\n";
        return false;
    }
    if (members_.isMember(who)) {
        cout << "    [提示] " << who->nick() << " 已在群内。\n";
        return false;
    }
    if (!who->account(svc_)) {
        cout << "    [提示] " << who->nick() << " 未开通 "
             << microName(svc_) << "，无法被邀请。\n";
        return false;
    }

    members_.add(who, ROLE_MEMBER);
    cout << "    [邀请] 成员 " << actor->nick() << " 邀请 " << who->nick()
         << " 加入[" << name_ << "]，成功。\n";
    return true;
}

bool Group::kick(User* actor, User* who) {
    if (!actor || !who) return false;
    if (!members_.isMember(actor)) {
        cout << "    [提示] " << actor->nick() << " 不是本群成员，无权操作。\n";
        return false;
    }
    if (!canKick(actor, who)) {
        cout << "    [提示] " << actor->nick() << " 无权踢出 " << who->nick()
             << "。" << kindName() << "规则：" << manageMode() << "\n";
        return false;
    }

    members_.remove(who);
    cout << "    [踢人] " << actor->nick() << " 已将 " << who->nick()
         << " 移出群[" << name_ << "]。\n";
    return true;
}

bool Group::setAdmin(User* actor, User* who) {
    if (!canSetAdmin(actor, who)) {
        cout << "    [提示] " << (actor ? actor->nick() : string("?"))
             << " 无权设置管理员。" << kindName()
             << "规则：" << manageMode() << "\n";
        return false;
    }

    members_.setRole(who, ROLE_ADMIN);
    cout << "    [设管理员] " << who->nick() << " 已成为["
         << name_ << "]管理员。\n";
    return true;
}

bool Group::cancelAdmin(User* actor, User* who) {
    if (!canSetAdmin(actor, who)) {
        cout << "    [提示] 无权取消管理员。" << kindName()
             << "规则：" << manageMode() << "\n";
        return false;
    }

    members_.setRole(who, ROLE_MEMBER);
    cout << "    [取消管理员] " << who->nick() << " 已变为普通成员。\n";
    return true;
}

// 群主不能直接退群：否则群会没有群主。先转让群主再退。
bool Group::quit(User* who) {
    if (!who || !members_.isMember(who)) {
        cout << "    [提示] 非群成员不能退群。\n";
        return false;
    }
    if (who == owner()) {
        cout << "    [提示] 群主不能直接退群（可先转让群主）。\n";
        return false;
    }

    members_.remove(who);
    cout << "    [退群] " << who->nick() << " 已退出[" << name_ << "]。\n";
    return true;
}

void Group::show() const {
    User* ow = owner();
    cout << "  ------- " << kindName() << " " << gno_ << " -------\n";
    cout << "   群号：" << gno_ << "  群名：" << name_
         << "  所属：" << microName(svc_) << "\n";
    cout << "   群主：" << (ow ? ow->nick() : string("(无)")) << "\n";
    cout << "   群人数：" << members_.count() << "\n";
    cout << "   管理模式：" << manageMode() << "\n";
    members_.list();
    cout << "  ---------------------------------\n";
}
