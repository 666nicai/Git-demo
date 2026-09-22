// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：MicroAccountWRH.h
#ifndef MICROACCOUNT_WRH_H
#define MICROACCOUNT_WRH_H

#include "ConstWRH.h"
#include "MyDateWRH.h"
#include "FriendBookWRH.h"
#include <string>

class UserWRH;   // 前向声明

// =====================================================================
// MicroAccountWRH  微X账号信息类（抽象基类）
// 代表"某平台用户在某个微X服务上的账号"，是被用户(自然人)开通的服务载体。
// 继承族体现出题目第1点的两种号码ID策略：
//     - QQ、微博：共享ID（isSharedID()==true，号码 = 主号）
//     - 微信：独立ID，可与QQ绑定（isSharedID()==false）
// 每个服务账号内部维护"该服务下"的好友列表（互不相同 → 才能体现
// 微X间共同好友/微信加QQ推荐好友）
// =====================================================================
class MicroAccountWRH {
protected:
    UserWRH*     owner_;       // 归属的平台用户
    MicroType    type_;        // 服务类型
    bool         logged_;      // 是否已登录
    MyDateWRH    applyDate_;   // 本号码申请时间(T龄)
    FriendBookWRH friends_;    // 该服务下我的好友列表
public:
    MicroAccountWRH(UserWRH* owner, MicroType t, const MyDateWRH& apply);
    virtual ~MicroAccountWRH();

    UserWRH* owner() const { return owner_; }
    MicroType type() const { return type_; }
    bool      logged() const { return logged_; }
    void      setLogged(bool b) { logged_ = b; }
    const MyDateWRH& applyDate() const { return applyDate_; }
    long      tAge() const { return MyDateWRH::today().diff(applyDate_); }

    // ---- 好友（该服务下）----
    FriendBookWRH& friends() { return friends_; }
    const FriendBookWRH& friends() const { return friends_; }
    // 添加/删除好友（互为好友；对方需开通同服务）
    bool addFriend(UserWRH* who, const std::string& remark);
    bool delFriend(UserWRH* who);
    bool isFriend(UserWRH* who) const { return friends_.has(who); }

    // ---- 多态：微X账号的区别 ----
    virtual const char* svcName()   const = 0;  // "QQ"/"微博"/"微信"
    virtual bool        isSharedID() const = 0; // QQ/微博 共享=true；微信=false
    virtual bool        isWeChat()   const = 0; // 是否微信(独立ID可绑定)
    virtual std::string svcNo()      const = 0; // 账号号码
    // 展示该账号信息
    virtual void show() const;
};

#endif
