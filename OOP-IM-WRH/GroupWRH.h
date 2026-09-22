// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：GroupWRH.h
#ifndef GROUP_WRH_H
#define GROUP_WRH_H

#include "ConstWRH.h"
#include "GroupMemberBookWRH.h"
#include <string>
#include <vector>

class UserWRH;

// =====================================================================
// GroupWRH  群信息抽象基类（多态：QQ群 vs 微信群）
// ---------------------------------------------------------------------
// 群主制核心概念：
//   预置群号 1001~1006；群成员容器 members_ 存成员及其角色(群主/管理员/成员)。
// 本基类以"模板方法"方式把公共流程固定下来，把各微X群的差异交给虚函数
// (canApplyJoin/canKick/setAdmin …) 由派生类 QQGroupWRH / WeChatGroupWRH 实现。
// =====================================================================
class GroupWRH {
protected:
    int             gno_;       // 群号
    std::string     name_;      // 群名称
    MicroType       svc_;       // 所属微X服务
    GroupMemberBookWRH members_; // 群成员容器（含角色）
public:
    GroupWRH(int gno, const std::string& name, MicroType svc, UserWRH* owner);
    virtual ~GroupWRH() {}

    // ---- 基本信息 ----
    int         gno()  const { return gno_; }
    const std::string& name() const { return name_; }
    MicroType   svc()  const { return svc_; }
    int         memberCount() const { return members_.count(); }
    GroupMemberBookWRH& members() { return members_; }
    const GroupMemberBookWRH& members() const { return members_; }

    UserWRH*    owner() const;              // 群主

    // ---- 多态差异接口（QQ群/微信群各自实现）----
    virtual const char* kindName()    const = 0; // "QQ群" / "微信群"
    virtual bool canApplyJoin()   const = 0; // QQ=true(可申请) 微信=false(仅推荐/邀请)
    virtual bool canCreateSub()   const = 0; // QQ=true(临时讨论组) 微信=false
    virtual bool hasAdminRole()   const = 0; // QQ=true 微信=false(群主为核心)
    virtual bool canKick(const UserWRH* actor, const UserWRH* who) const = 0;
    virtual bool canSetAdmin(const UserWRH* actor, const UserWRH* who) const = 0;
    virtual std::string manageMode() const = 0;  // 管理模式文字说明
    // 建"临时讨论组/子群"（QQ群支持；微信群不支持 → 返回 null 并给提示）
    virtual GroupWRH* createSubGroup(int subNo, const std::string& subName,
                                     const std::vector<UserWRH*>& picks) const = 0;

    // ---- 群管理模式动态切换（QQ风格<->微信风格，成员数据不受伤）----
    virtual GroupWRH* modeTwin() const = 0;  // 生成管理风格相反的同名新群

    // ---- 公共群管理流程（模板方法）----
    bool join(UserWRH* who);                 // 加入：QQ=可申请 微信=仅推荐，故见注释
    bool requestJoin(UserWRH* who);          // 申请加入（按 canApplyJoin 判定）
    bool inviteJoin(UserWRH* actor, UserWRH* who); // 成员邀请/推荐加入
    bool kick(UserWRH* actor, UserWRH* who);       // 踢人（按 canKick 判定）
    bool setAdmin(UserWRH* actor, UserWRH* who);   // 设管理员（按 canSetAdmin 判定）
    bool cancelAdmin(UserWRH* actor, UserWRH* who);
    bool quit(UserWRH* who);                 // 退群（群主除外）
    bool isMember(UserWRH* u) const { return members_.isMember(u); }

    void show() const;   // 展示群信息与成员
};

#endif
