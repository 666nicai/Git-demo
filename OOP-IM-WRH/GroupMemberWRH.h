// ============================================================
// 文件名: GroupMemberWRH.h
// 描述  : 群成员记录——成员本人 + 他在群里的角色
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef GROUPMEMBER_WRH_H
#define GROUPMEMBER_WRH_H

#include "ConstWRH.h"

class UserWRH;   // 前向声明

// 一条群成员记录。
// QQ群有 群主/管理员/成员 三种角色；微信群实际上只会出现 群主/成员。
class GroupMemberWRH {
private:
    UserWRH*   user_;   // 成员本人
    MemberRole role_;   // 他在群里的角色

public:
    GroupMemberWRH(UserWRH* u = nullptr, MemberRole r = ROLE_MEMBER)
        : user_(u), role_(r) {}

    UserWRH* user() const { return user_; }
    void setUser(UserWRH* u) { user_ = u; }

    MemberRole role() const { return role_; }
    void setRole(MemberRole r) { role_ = r; }

    bool isOwner() const { return role_ == ROLE_OWNER; }
    bool isAdmin() const { return role_ == ROLE_ADMIN; }
};

#endif
