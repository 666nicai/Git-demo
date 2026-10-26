// ============================================================
// 文件名: WeChatGroupWRH.h
// 描述  : 微信群类——只能被邀请、仅群主特权、无临时讨论组
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef WGROUP_WRH_H
#define WGROUP_WRH_H

#include "GroupWRH.h"

class UserWRH;

// 微信群规则：
//   · 不能主动申请，只能由群成员邀请/推荐
//   · 没有管理员，只有群主是特权账号
//   · 只有群主能踢人
//   · 不支持临时讨论组
class WeChatGroupWRH : public GroupWRH {
public:
    WeChatGroupWRH(int gno, const std::string& name, UserWRH* owner)
        : GroupWRH(gno, name, MWX, owner) {}

    const char* kindName()     const override { return "微信群"; }
    bool canApplyJoin()        const override { return false; }
    bool canCreateSub()        const override { return false; }
    bool hasAdminRole()        const override { return false; }

    bool canKick(const UserWRH* actor, const UserWRH* who) const override;
    bool canSetAdmin(const UserWRH* actor, const UserWRH* who) const override;
    std::string manageMode() const override;

    GroupWRH* createSubGroup(int subNo, const std::string& subName,
                             const std::vector<UserWRH*>& picks) const override;
    GroupWRH* modeTwin() const override;   // 切到QQ群规则
};

#endif
