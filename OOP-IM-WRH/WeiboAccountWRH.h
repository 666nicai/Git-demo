// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：WeiboAccountWRH.h
#ifndef WEIBOACCOUNT_WRH_H
#define WEIBOACCOUNT_WRH_H

#include "MicroAccountWRH.h"

class UserWRH;

// =====================================================================
// WeiboAccountWRH  微博账号信息类（微X账号继承族之一）
// 微博与QQ共享ID：号码 = 平台主号（与 QQAccount 相同号码）
// 这正体现"微博与QQ共享ID"：两者 svcNo() 返回同一个主号
// =====================================================================
class WeiboAccountWRH : public MicroAccountWRH {
public:
    WeiboAccountWRH(UserWRH* owner, const MyDateWRH& apply)
        : MicroAccountWRH(owner, MWB, apply) {}

    virtual const char* svcName() const override { return "微博"; }
    virtual bool isSharedID() const override { return true; }
    virtual bool isWeChat() const override { return false; }
    virtual std::string svcNo() const override;
};

#endif
