// ============================================================
// 文件名: GroupMemberBookWRH.h
// 描述  : 群成员容器——一个群内全部成员及角色
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef GROUPMEMBERBOOK_WRH_H
#define GROUPMEMBERBOOK_WRH_H

#include "GroupMemberWRH.h"
#include <vector>

class UserWRH;

// 管理一个群里的所有成员：加入、移出、改角色、查询、展示。
class GroupMemberBookWRH {
private:
    std::vector<GroupMemberWRH> v_;

public:
    GroupMemberBookWRH() {}

    int  count() const { return (int)v_.size(); }
    bool empty() const { return v_.empty(); }

    bool add(UserWRH* u, MemberRole r = ROLE_MEMBER);   // 已在群内则返回 false
    bool remove(UserWRH* u);
    bool isMember(UserWRH* u) const;

    GroupMemberWRH*       find(UserWRH* u);
    const GroupMemberWRH* find(UserWRH* u) const;

    bool setRole(UserWRH* u, MemberRole r);

    std::vector<UserWRH*> users() const;   // 只取成员，不带角色
    const std::vector<GroupMemberWRH>& all() const { return v_; }

    void list() const;   // 展示成员与角色（需要 UserWRH 完整定义，放 .cpp）
};

#endif
