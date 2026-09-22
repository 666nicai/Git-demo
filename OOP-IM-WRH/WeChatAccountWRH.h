// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：WeChatAccountWRH.h
#ifndef WECHATACCOUNT_WRH_H
#define WECHATACCOUNT_WRH_H

#include "MicroAccountWRH.h"

class UserWRH;

// =====================================================================
// WeChatAccountWRH  微信账号信息类（微X账号继承族之一）
// 微信与QQ/微博不同：采用独立ID（微信号），可与QQ号码绑定。
// svcNo() 返回微信独立号；boundQQ() 返回绑定的QQ主号（可为空=未绑定）。
// =====================================================================
class WeChatAccountWRH : public MicroAccountWRH {
private:
    std::string wxNo_;    // 独立微信号
    std::string boundQQ_; // 绑定的QQ主号（空串 = 未绑定）
public:
    WeChatAccountWRH(UserWRH* owner, const MyDateWRH& apply);

    virtual const char* svcName() const override { return "微信"; }
    virtual bool isSharedID() const override { return false; }
    virtual bool isWeChat() const override { return true; }
    virtual std::string svcNo() const override;   // 返回独立微信号
    virtual void show() const override;           // 追加显示绑定状态

    // ---- 独立号 + QQ绑定管理 ----
    const std::string& wxNo() const { return wxNo_; }
    void setWxNo(const std::string& w) { wxNo_ = w; }
    bool hasBoundQQ() const { return !boundQQ_.empty(); }
    const std::string& boundQQ() const { return boundQQ_; }
    void bindQQ(const std::string& qq) { boundQQ_ = qq; }
    void unbindQQ() { boundQQ_.clear(); }
};

#endif
