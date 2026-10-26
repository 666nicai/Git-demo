// ============================================================
// 文件名: WeiboAccountWRH.h
// 描述  : 微博账号类——与QQ共享平台主号
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef WEIBOACCOUNT_WRH_H
#define WEIBOACCOUNT_WRH_H

#include "MicroAccountWRH.h"

class UserWRH;

// 微博与QQ共享ID：两者的 svcNo() 返回同一个主号，
// 这正是题目里“微博与QQ共享ID”的体现。
class WeiboAccountWRH : public MicroAccountWRH {
public:
    WeiboAccountWRH(UserWRH* owner, const MyDateWRH& apply)
        : MicroAccountWRH(owner, MWB, apply) {}

    const char* svcName()    const override { return "微博"; }
    bool        isSharedID() const override { return true; }
    bool        isWeChat()   const override { return false; }
    std::string svcNo()      const override;   // 返回平台主号
};

#endif
