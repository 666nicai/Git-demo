// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：QQAccountWRH.h
#ifndef QQACCOUNT_WRH_H
#define QQACCOUNT_WRH_H

#include "MicroAccountWRH.h"

class UserWRH;

// =====================================================================
// QQAccountWRH  QQ账号信息类（微X账号继承族之一）
// QQ 与微博共享ID：QQ号码 = 平台主号
// =====================================================================
class QQAccountWRH : public MicroAccountWRH {
public:
    QQAccountWRH(UserWRH* owner, const MyDateWRH& apply)
        : MicroAccountWRH(owner, MQQ, apply) {}

    virtual const char* svcName() const override { return "QQ"; }
    virtual bool isSharedID() const override { return true; }
    virtual bool isWeChat() const override { return false; }
    // QQ号码：直接返回平台主号（共享）
    virtual std::string svcNo() const override;
};

#endif
