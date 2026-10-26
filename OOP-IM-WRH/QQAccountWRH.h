// ============================================================
// 文件名: QQAccountWRH.h
// 描述  : QQ账号类——与微博共享平台主号
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef QQACCOUNT_WRH_H
#define QQACCOUNT_WRH_H

#include "MicroAccountWRH.h"

class UserWRH;

// QQ 与微博共享ID，所以 QQ 号码直接取平台主号。
class QQAccountWRH : public MicroAccountWRH {
public:
    QQAccountWRH(UserWRH* owner, const MyDateWRH& apply)
        : MicroAccountWRH(owner, MQQ, apply) {}

    const char* svcName()    const override { return "QQ"; }
    bool        isSharedID() const override { return true; }
    bool        isWeChat()   const override { return false; }
    std::string svcNo()      const override;   // 返回平台主号
};

#endif
