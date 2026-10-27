// ============================================================
// 文件名: MicroAccountWRH.h
// 描述  : 微X账号抽象基类——某人在某个服务上的账号
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef MICROACCOUNT_WRH_H
#define MICROACCOUNT_WRH_H

#include <bits/stdc++.h>

#include "ConstWRH.h"
#include "FriendBookWRH.h"
#include "MyDateWRH.h"

class UserWRH;   // 前向声明

// 微X账号抽象基类。
// 一个自然人开通一个服务，就多一个 MicroAccountWRH 派生对象。
// 号码策略由派生类决定：
//   QQ、微博：isSharedID() == true，号码就是平台主号
//   微信    ：isSharedID() == false，用自己的微信号，可以绑定QQ
// 每个账号各自维护好友列表，所以“QQ好友”和“微信好友”互不混在一起。
class MicroAccountWRH {
protected:
    UserWRH*     owner_;      // 账号属于谁
    MicroType    type_;       // 哪个服务
    bool         logged_;     // 是否已登录
    MyDateWRH    applyDate_;  // 本号码申请时间（算 T 龄）
    FriendBookWRH friends_;   // 本服务下的好友列表

public:
    MicroAccountWRH(UserWRH* owner, MicroType t, const MyDateWRH& apply);
    virtual ~MicroAccountWRH();

    // 账号对象代表“某人的某个号”，复制没有意义，直接禁止
    MicroAccountWRH(const MicroAccountWRH&) = delete;
    MicroAccountWRH& operator=(const MicroAccountWRH&) = delete;

    UserWRH*  owner() const { return owner_; }
    MicroType type() const { return type_; }
    bool      logged() const { return logged_; }
    void      setLogged(bool b) { logged_ = b; }
    const MyDateWRH& applyDate() const { return applyDate_; }
    long      tAge() const { return MyDateWRH::today().diff(applyDate_); }

    // ---- 好友（本服务下）----
    FriendBookWRH& friends() { return friends_; }
    const FriendBookWRH& friends() const { return friends_; }

    // 添加/删除好友：双方互为好友，对方也必须开通同一服务
    bool addFriend(UserWRH* who, const std::string& remark);
    bool delFriend(UserWRH* who);
    bool isFriend(UserWRH* who) const { return friends_.has(who); }

    // ---- 多态差异：QQ / 微博 / 微信各自实现 ----
    virtual const char* svcName()    const = 0;   // "QQ" / "微博" / "微信"
    virtual bool        isSharedID() const = 0;   // QQ、微博 = true；微信 = false
    virtual bool        isWeChat()   const = 0;   // 是否微信
    virtual std::string svcNo()      const = 0;   // 本服务的号码
    virtual void        show()       const;       // 公共账号信息
};

#endif
