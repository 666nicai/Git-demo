// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：QQGroupWRH.cpp
#include "QQGroupWRH.h"
#include "UserWRH.h"
#include "WeChatGroupWRH.h"
#include <iostream>

// =====================================================================
// QQGroupWRH.cpp  QQ群实现
// =====================================================================

// 踢人权限：群主可踢除任何普通成员/管理员；管理员只能踢普通成员
bool QQGroupWRH::canKick(const UserWRH* actor, const UserWRH* who) const {
    if (!actor || !who || actor == who) return false;
    if (who == owner()) return false;                 // 群主不可被踢
    const GroupMemberWRH* a = members().find(const_cast<UserWRH*>(actor));
    const GroupMemberWRH* w = members().find(const_cast<UserWRH*>(who));
    if (!a || !w) return false;
    if (a->isOwner()) return true;                    // 群主踢任何人
    if (a->isAdmin() && !w->isOwner() && !w->isAdmin())
        return true;                                  // 管理员只能踢普通成员
    return false;
}

// 设管理员权限：仅群主可为"普通成员"设管理员
bool QQGroupWRH::canSetAdmin(const UserWRH* actor, const UserWRH* who) const {
    if (!actor || !who || actor != owner()) return false;   // 群主专属
    if (who == owner()) return false;
    const GroupMemberWRH* w = members().find(const_cast<UserWRH*>(who));
    return w && !w->isOwner();
}

std::string QQGroupWRH::manageMode() const {
    return "群主为核心的管理员制度：群主/管理员共同管理；成员可主动申请加入；"
           "群主可设/撤管理员；支持临时讨论组";
}

// QQ群可建"临时讨论组(子群)"：选出部分成员组建子群
GroupWRH* QQGroupWRH::createSubGroup(int subNo, const std::string& subName,
                                     const std::vector<UserWRH*>& picks) const {
    QQGroupWRH* sub = new QQGroupWRH(subNo, subName, owner());
    for (size_t i = 0; i < picks.size(); ++i)
        if (picks[i] && picks[i] != owner())
            sub->members().add(picks[i], ROLE_MEMBER);
    return sub;
}

// 切换管理模式：生成一个数据相同、但采用微信管理规则的"同群"对象
GroupWRH* QQGroupWRH::modeTwin() const {
    WeChatGroupWRH* ng = new WeChatGroupWRH(gno_, name_, owner());
    const std::vector<GroupMemberWRH>& v = members().all();
    for (size_t i = 0; i < v.size(); ++i) {
        UserWRH* u = v[i].user();
        if (u && u != owner())
            ng->members().add(u, ROLE_MEMBER);   // 原管理员在微信群中降为普通成员
    }
    return ng;
}
