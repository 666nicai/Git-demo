// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：GroupMemberWRH.h
#ifndef GROUPMEMBER_WRH_H
#define GROUPMEMBER_WRH_H

#include "ConstWRH.h"
#include <string>

class UserWRH;   // 前向声明

// =====================================================================
// GroupMemberWRH  群成员信息类
// 一条群成员记录：成员(UserWRH*) + 其在群中的角色
// QQ群支持 群主/管理员/成员；微信群仅有 群主 为特权账号
// =====================================================================
class GroupMemberWRH {
private:
    UserWRH*   user_;
    MemberRole role_;
public:
    GroupMemberWRH(UserWRH* u = nullptr, MemberRole r = ROLE_MEMBER)
        : user_(u), role_(r) {}

    UserWRH*   user() const { return user_; }
    void       setUser(UserWRH* u) { user_ = u; }

    MemberRole role() const { return role_; }
    void       setRole(MemberRole r) { role_ = r; }

    bool       isOwner() const { return role_ == ROLE_OWNER; }
    bool       isAdmin() const { return role_ == ROLE_ADMIN; }
};

#endif
