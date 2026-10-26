// ============================================================
// 文件名: WeChatGroupWRH.cpp
// 描述  : 微信群实现——仅群主特权、无管理员、无子群、模式切换
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "WeChatGroupWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"
#include "QQGroupWRH.h"
#include <iostream>

using namespace std;

// 踢人规则：只有群主能踢，且不能踢自己
bool WeChatGroup::canKick(const User* actor, const User* who) const {
    if (!actor || !who || actor == who) return false;
    if (actor != owner()) return false;
    if (who == owner()) return false;
    return members().isMember(const_cast<User*>(who));
}

// 微信群没有管理员制度
bool WeChatGroup::canSetAdmin(const User* actor, const User* who) const {
    (void)actor;
    (void)who;
    return false;
}

string WeChatGroup::manageMode() const {
    return "仅群主为特权账号：成员只能受邀/推荐加入；无管理员；"
           "仅群主可踢人；不支持临时讨论组";
}

// 微信群不支持临时讨论组
Group* WeChatGroup::createSubGroup(int subNo, const string& subName,
                                         const vector<User*>& picks) const {
    (void)subNo;
    (void)subName;
    (void)picks;
    cout << "    [提示] 微信群不支持创建临时讨论组(子群)。\n";
    return nullptr;
}

// 动态切换管理模式：新建同号同名、规则为QQ的群。
// 成员原样搬过去，人数不变；原群主仍是群主，其他人一律普通成员。
Group* WeChatGroup::modeTwin() const {
    User* ow = owner();
    if (!ow) return nullptr;

    QQGroup* ng = new QQGroup(gno_, name_, ow);
    const vector<GroupMember>& v = members().all();
    for (size_t i = 0; i < v.size(); ++i) {
        User* u = v[i].user();
        if (u && u != ow)
            ng->members().add(u, ROLE_MEMBER);
    }
    return ng;
}
