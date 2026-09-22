// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：QQGroupWRH.h
#ifndef QGROUP_WRH_H
#define QGROUP_WRH_H

#include "GroupWRH.h"

class UserWRH;

// =====================================================================
// QQGroupWRH  QQ群类（群多态族之QQ实现）
//   · 可申请加入   · 有管理员制度   · 可建临时讨论组(子群)
//   · 踢人：群主/管理员可踢普通成员
// =====================================================================
class QQGroupWRH : public GroupWRH {
public:
    QQGroupWRH(int gno, const std::string& name, UserWRH* owner)
        : GroupWRH(gno, name, MQQ, owner) {}

    virtual const char* kindName() const override { return "QQ群"; }
    virtual bool canApplyJoin() const override { return true; }  // 可主动申请
    virtual bool canCreateSub() const override { return true; }  // 可建临时讨论组
    virtual bool hasAdminRole() const override { return true; }  // 有管理员制度

    virtual bool canKick(const UserWRH* actor, const UserWRH* who) const override;
    virtual bool canSetAdmin(const UserWRH* actor, const UserWRH* who) const override;
    virtual std::string manageMode() const override;
    virtual GroupWRH* createSubGroup(int subNo, const std::string& subName,
                                     const std::vector<UserWRH*>& picks) const override;
    // 切到微信管理模式（成员数据不受伤）
    virtual GroupWRH* modeTwin() const override;
};

#endif
