// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：WeChatGroupWRH.cpp
#include "WeChatGroupWRH.h"
#include "UserWRH.h"
#include "QQGroupWRH.h"
#include <iostream>

// =====================================================================
// WeChatGroupWRH.cpp  微信群实现
// =====================================================================

// 踢人权限：仅群主(特权账号)可踢，且不能踢群主自己
bool WeChatGroupWRH::canKick(const UserWRH* actor, const UserWRH* who) const {
    if (!actor || !who || actor == who) return false;
    if (actor != owner()) return false;               // 仅群主
    if (who == owner()) return false;                 // 不能踢群主
    return members().isMember(const_cast<UserWRH*>(who));
}

// 微信群没有管理员制度
bool WeChatGroupWRH::canSetAdmin(const UserWRH* actor, const UserWRH* who) const {
    (void)actor; (void)who;
    return false;
}

std::string WeChatGroupWRH::manageMode() const {
    return "仅群主为特权账号：成员只能受邀/推荐加入；无管理员；仅群主可踢人；"
           "不支持临时讨论组";
}

// 微信群不支持建临时讨论组
GroupWRH* WeChatGroupWRH::createSubGroup(int subNo, const std::string& subName,
                                         const std::vector<UserWRH*>& picks) const {
    (void)subNo; (void)subName; (void)picks;
    std::cout << "    [提示] 微信群不支持创建临时讨论组(子群)。\n";
    return nullptr;
}

// 切换管理模式：生成一个数据相同、但采用QQ管理规则的同群对象
GroupWRH* WeChatGroupWRH::modeTwin() const {
    QQGroupWRH* ng = new QQGroupWRH(gno_, name_, owner());
    const std::vector<GroupMemberWRH>& v = members().all();
    for (size_t i = 0; i < v.size(); ++i) {
        UserWRH* u = v[i].user();
        if (u && u != owner())
            ng->members().add(u, ROLE_MEMBER);
    }
    return ng;
}
