// ============================================================
// 文件名: QQGroupWRH.cpp
// 描述  : QQ群实现——管理员制度、踢人权限、临时讨论组、模式切换
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#include "QQGroupWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"
#include "WeChatGroupWRH.h"
#include <bits/stdc++.h>

using namespace std;

// 踢人规则：群主可踢群主以外的任何人；管理员只能踢普通成员
bool QQGroup::canKick(const User* actor, const User* who) const {
    if (!actor || !who || actor == who) return false;
    if (who == owner()) return false;   // 群主不可被踢

    const GroupMember* a = members().find(const_cast<User*>(actor));
    const GroupMember* w = members().find(const_cast<User*>(who));
    if (!a || !w) return false;

    if (a->isOwner()) return true;                       // 群主：谁都行
    if (a->isAdmin() && !w->isOwner() && !w->isAdmin())
        return true;                                     // 管理员：只能踢普通成员
    return false;
}

// 设/撤管理员：只有群主能操作，目标必须是群内成员且不是群主
bool QQGroup::canSetAdmin(const User* actor, const User* who) const {
    if (!actor || !who || actor != owner()) return false;
    if (who == owner()) return false;
    const GroupMember* w = members().find(const_cast<User*>(who));
    return w && !w->isOwner();
}

string QQGroup::manageMode() const {
    return "群主为核心的管理员制度：群主/管理员共同管理；成员可主动申请加入；"
           "群主可设/撤管理员；支持临时讨论组";
}

// 建临时讨论组：从原群成员里挑人，群主仍是原群主。
// 子群是真实的QQ群，所以被挑的人必须开通了QQ。
Group* QQGroup::createSubGroup(int subNo, const string& subName,
                                     const vector<User*>& picks) const {
    User* ow = owner();
    if (!ow) return nullptr;

    QQGroup* sub = new QQGroup(subNo, subName, ow);
    for (size_t i = 0; i < picks.size(); ++i) {
        User* p = picks[i];
        if (!p || p == ow || !p->account(MQQ)) continue;
        sub->members().add(p, ROLE_MEMBER);
    }
    return sub;
}

// 动态切换管理模式：新建一个同号同名、规则为微信的群。
// 成员原样搬过去（只演示规则变化，不再按微信服务校验），
// 所以成员数不变，体现“切换不伤成员数据”。
Group* QQGroup::modeTwin() const {
    User* ow = owner();
    if (!ow) return nullptr;

    WeChatGroup* ng = new WeChatGroup(gno_, name_, ow);
    const vector<GroupMember>& v = members().all();
    for (size_t i = 0; i < v.size(); ++i) {
        User* u = v[i].user();
        if (u && u != ow)
            ng->members().add(u, ROLE_MEMBER);   // 原管理员在微信群降为普通成员
    }
    return ng;
}
