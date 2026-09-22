// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：GroupMemberBookWRH.h
#ifndef GROUPMEMBERBOOK_WRH_H
#define GROUPMEMBERBOOK_WRH_H

#include "GroupMemberWRH.h"
#include <vector>

class UserWRH;

// =====================================================================
// GroupMemberBookWRH  群成员容器类（微X成员管理）
// 管理一个群内的全部成员记录：添加/删除/改角色/查询/展示
// =====================================================================
class GroupMemberBookWRH {
private:
    std::vector<GroupMemberWRH> v_;
public:
    GroupMemberBookWRH() {}

    int count() const { return (int)v_.size(); }
    bool empty() const { return v_.empty(); }

    // 添加成员（已在群内则返回 false）
    bool add(UserWRH* u, MemberRole r = ROLE_MEMBER);
    // 删除成员
    bool remove(UserWRH* u);
    // 查询
    bool isMember(UserWRH* u) const;
    GroupMemberWRH* find(UserWRH* u);
    const GroupMemberWRH* find(UserWRH* u) const;
    // 改角色
    bool setRole(UserWRH* u, MemberRole r);

    // 取所有成员（不含角色）
    std::vector<UserWRH*> users() const;
    const std::vector<GroupMemberWRH>& all() const { return v_; }

    // 展示成员（含角色），需要 UserWRH 完整定义 -> .cpp 实现
    void list() const;
};

#endif
