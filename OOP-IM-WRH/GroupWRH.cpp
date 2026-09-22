// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：GroupWRH.cpp
#include "GroupWRH.h"
#include "UserWRH.h"
#include <iostream>

// =====================================================================
// GroupWRH.cpp  群信息抽象基类实现
// =====================================================================

GroupWRH::GroupWRH(int gno, const std::string& name, MicroType svc, UserWRH* owner)
    : gno_(gno), name_(name), svc_(svc) {
    // 建群者即为群主（ROLE_OWNER），自动进入成员容器
    if (owner) members_.add(owner, ROLE_OWNER);
}

UserWRH* GroupWRH::owner() const {
    const std::vector<GroupMemberWRH>& v = members_.all();
    for (size_t i = 0; i < v.size(); ++i)
        if (v[i].role() == ROLE_OWNER) return v[i].user();
    return nullptr;
}

// 申请加入：QQ群可申请；微信群只能邀请/推荐，故申请被拒并给出提示
bool GroupWRH::requestJoin(UserWRH* who) {
    if (!who) return false;
    if (members_.isMember(who)) {
        std::cout << "    [提示] " << who->nick() << " 已在本群。\n";
        return false;
    }
    if (!who->account(svc_)) {
        std::cout << "    [提示] " << who->nick() << " 未开通 " << microName(svc_)
                  << "，不能加入该" << kindName() << "。\n";
        return false;
    }
    if (canApplyJoin()) {
        members_.add(who, ROLE_MEMBER);
        std::cout << "    [入群] " << who->nick() << " 申请成功，已加入["
                  << name_ << "]。\n";
        return true;
    }
    std::cout << "    [拒绝] " << kindName() << "不支持主动申请加入，只能由"
              << microName(svc_) << "成员 推荐/邀请 加入。\n";
    return false;
}

// 成员邀请/推荐加入（两种群都允许；微信借此实现"推荐入群"）
bool GroupWRH::inviteJoin(UserWRH* actor, UserWRH* who) {
    if (!actor || !who) return false;
    if (!members_.isMember(actor)) {
        std::cout << "    [提示] " << actor->nick() << " 不是本群成员，无权邀请。\n";
        return false;
    }
    if (members_.isMember(who)) {
        std::cout << "    [提示] " << who->nick() << " 已在群内。\n";
        return false;
    }
    if (!who->account(svc_)) {
        std::cout << "    [提示] " << who->nick() << " 未开通 " << microName(svc_)
                  << "，无法被邀请。\n";
        return false;
    }
    members_.add(who, ROLE_MEMBER);
    std::cout << "    [邀请] 成员 " << actor->nick() << " 邀请 " << who->nick()
              << " 加入[" << name_ << "]，成功。\n";
    return true;
}

// 踢人：QQ群=群主与管理员可踢普通成员；微信群=仅群主(特权账号)可踢
bool GroupWRH::kick(UserWRH* actor, UserWRH* who) {
    if (!actor || !who) return false;
    if (!members_.isMember(actor)) {
        std::cout << "    [提示] " << actor->nick() << " 不是本群成员，无权操作。\n";
        return false;
    }
    if (!canKick(actor, who)) {
        std::cout << "    [提示] " << actor->nick() << " 无权踢出 " << who->nick()
                  << "。" << kindName() << "规则：" << manageMode() << "\n";
        return false;
    }
    members_.remove(who);
    std::cout << "    [踢人] " << actor->nick() << " 已将 " << who->nick()
              << " 移出群[" << name_ << "]。\n";
    return true;
}

// 设管理员：QQ群=群主可设；微信群=无管理员，仅群主
bool GroupWRH::setAdmin(UserWRH* actor, UserWRH* who) {
    if (!canSetAdmin(actor, who)) {
        std::cout << "    [提示] " << (actor ? actor->nick() : std::string("?"))
                  << " 无权设置管理员。" << kindName() << "规则：" << manageMode() << "\n";
        return false;
    }
    members_.setRole(who, ROLE_ADMIN);
    std::cout << "    [设管理员] " << who->nick() << " 已成为[" << name_ << "]管理员。\n";
    return true;
}

bool GroupWRH::cancelAdmin(UserWRH* actor, UserWRH* who) {
    if (!canSetAdmin(actor, who)) {
        std::cout << "    [提示] 无权取消管理员。" << kindName() << "规则：" << manageMode() << "\n";
        return false;
    }
    members_.setRole(who, ROLE_MEMBER);
    std::cout << "    [取消管理员] " << who->nick() << " 已变为普通成员。\n";
    return true;
}

// 退群：群主不允许直接退群（可由群主转让后退出）
bool GroupWRH::quit(UserWRH* who) {
    if (!who || !members_.isMember(who)) {
        std::cout << "    [提示] 非群成员不能退群。\n";
        return false;
    }
    if (who == owner()) {
        std::cout << "    [提示] 群主不能直接退群（可先转让群主）。\n";
        return false;
    }
    members_.remove(who);
    std::cout << "    [退群] " << who->nick() << " 已退出[" << name_ << "]。\n";
    return true;
}

// 展示群信息及成员
void GroupWRH::show() const {
    UserWRH* ow = owner();
    std::cout << "  ------- " << kindName() << " " << gno_ << " -------\n";
    std::cout << "   群号：" << gno_ << "  群名：" << name_
              << "  所属：" << microName(svc_) << "\n";
    std::cout << "   群主：" << (ow ? ow->nick() : std::string("(无)")) << "\n";
    std::cout << "   群人数：" << members_.count() << "\n";
    std::cout << "   管理模式：" << manageMode() << "\n";
    members_.list();
    std::cout << "  ---------------------------------\n";
}
