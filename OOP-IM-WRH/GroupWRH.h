// ============================================================
// 文件名: GroupWRH.h
// 描述  : 群抽象基类——模板方法固定流程，派生类决定QQ/微信差异
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef GROUP_WRH_H
#define GROUP_WRH_H

#include "ConstWRH.h"
#include "GroupMemberBookWRH.h"
#include <string>
#include <vector>

class UserWRH;

// 群抽象基类。
// 公共流程（申请、邀请、踢人、设管理员、退群、展示）写在基类里，
// 具体“能不能做”交给纯虚函数，由 QQGroupWRH / WeChatGroupWRH 实现。
// 这就是模板方法模式：流程不变，规则可变。
//
// 群号规则：每个服务预置 1001~1006；QQ 临时讨论组从 2001 往后排。
class GroupWRH {
protected:
    int               gno_;      // 群号
    std::string       name_;     // 群名
    MicroType         svc_;      // 属于哪个服务
    GroupMemberBookWRH members_; // 成员及角色

public:
    GroupWRH(int gno, const std::string& name, MicroType svc, UserWRH* owner);
    virtual ~GroupWRH() {}

    // 群对象由平台用指针管理，复制会带来成员容器的重复归属，直接禁止
    GroupWRH(const GroupWRH&) = delete;
    GroupWRH& operator=(const GroupWRH&) = delete;

    // ---- 基本信息 ----
    int         gno()  const { return gno_; }
    const std::string& name() const { return name_; }
    MicroType   svc()  const { return svc_; }
    int         memberCount() const { return members_.count(); }
    GroupMemberBookWRH& members() { return members_; }
    const GroupMemberBookWRH& members() const { return members_; }
    UserWRH*    owner() const;

    // ---- 多态差异：QQ群 / 微信群各自实现 ----
    virtual const char* kindName()   const = 0;   // "QQ群" / "微信群"
    virtual bool canApplyJoin()      const = 0;   // QQ=true；微信=false
    virtual bool canCreateSub()      const = 0;   // QQ=true；微信=false
    virtual bool hasAdminRole()      const = 0;   // QQ=true；微信=false
    virtual bool canKick(const UserWRH* actor, const UserWRH* who) const = 0;
    virtual bool canSetAdmin(const UserWRH* actor, const UserWRH* who) const = 0;
    virtual std::string manageMode() const = 0;   // 管理模式说明

    // 建临时讨论组 / 子群：QQ群支持，微信群返回空指针
    virtual GroupWRH* createSubGroup(int subNo, const std::string& subName,
                                     const std::vector<UserWRH*>& picks) const = 0;

    // 动态切换管理模式：生成成员相同、规则相反的同号群
    virtual GroupWRH* modeTwin() const = 0;

    // ---- 公共流程（模板方法）----
    bool join(UserWRH* who);                       // 入群：按 canApplyJoin 走
    bool requestJoin(UserWRH* who);                // 主动申请
    bool inviteJoin(UserWRH* actor, UserWRH* who); // 成员邀请/推荐
    bool kick(UserWRH* actor, UserWRH* who);
    bool setAdmin(UserWRH* actor, UserWRH* who);
    bool cancelAdmin(UserWRH* actor, UserWRH* who);
    bool quit(UserWRH* who);
    bool isMember(UserWRH* u) const { return members_.isMember(u); }

    void show() const;
};

#endif
