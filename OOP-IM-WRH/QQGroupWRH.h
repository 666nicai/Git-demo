// ============================================================
// 文件名: QQGroupWRH.h
// 描述  : QQ群类——可申请、有管理员、可建临时讨论组
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef QGROUP_WRH_H
#define QGROUP_WRH_H

#include "GroupWRH.h"

class UserWRH;

// QQ群规则：
//   · 成员可以主动申请加入
//   · 有管理员制度，群主可以设/撤管理员
//   · 群主可以踢任何人（群主自己除外）；管理员只能踢普通成员
//   · 支持临时讨论组（子群）
class QQGroupWRH : public GroupWRH {
public:
    QQGroupWRH(int gno, const std::string& name, UserWRH* owner)
        : GroupWRH(gno, name, MQQ, owner) {}

    const char* kindName()     const override { return "QQ群"; }
    bool canApplyJoin()        const override { return true; }
    bool canCreateSub()        const override { return true; }
    bool hasAdminRole()        const override { return true; }

    bool canKick(const UserWRH* actor, const UserWRH* who) const override;
    bool canSetAdmin(const UserWRH* actor, const UserWRH* who) const override;
    std::string manageMode() const override;

    GroupWRH* createSubGroup(int subNo, const std::string& subName,
                             const std::vector<UserWRH*>& picks) const override;
    GroupWRH* modeTwin() const override;   // 切到微信群规则
};

#endif
