// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：WeChatGroupWRH.h
#ifndef WGROUP_WRH_H
#define WGROUP_WRH_H

#include "GroupWRH.h"

class UserWRH;

// =====================================================================
// WeChatGroupWRH  微信群类（群多态族之微信实现）
//   · 只能由成员 推荐/邀请 加入，不能主动申请
//   · 无管理员制度，仅"群主"为特权账号
//   · 不支持临时讨论组(子群)
//   · 踢人：仅群主可踢
// =====================================================================
class WeChatGroupWRH : public GroupWRH {
public:
    WeChatGroupWRH(int gno, const std::string& name, UserWRH* owner)
        : GroupWRH(gno, name, MWX, owner) {}

    virtual const char* kindName() const override { return "微信群"; }
    virtual bool canApplyJoin() const override { return false; } // 只能推荐/邀请
    virtual bool canCreateSub() const override { return false; } // 无临时讨论组
    virtual bool hasAdminRole() const override { return false; } // 仅群主特权

    virtual bool canKick(const UserWRH* actor, const UserWRH* who) const override;
    virtual bool canSetAdmin(const UserWRH* actor, const UserWRH* who) const override;
    virtual std::string manageMode() const override;
    virtual GroupWRH* createSubGroup(int subNo, const std::string& subName,
                                     const std::vector<UserWRH*>& picks) const override;
    // 切到QQ管理模式（成员数据不受伤）
    virtual GroupWRH* modeTwin() const override;
};

#endif
