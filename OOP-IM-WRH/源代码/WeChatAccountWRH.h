// ============================================================
// 文件名: WeChatAccountWRH.h
// 描述  : 微信账号类——独立微信号，可与QQ绑定
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef WECHATACCOUNT_WRH_H
#define WECHATACCOUNT_WRH_H

#include <bits/stdc++.h>

#include "MicroAccountWRH.h"

class UserWRH;

// 微信和QQ/微博不同：用独立微信号，不共享主号。
// 另外多存一个 boundQQ_，表示这个微信号绑定了哪个QQ号（空串 = 未绑定）。
class WeChatAccountWRH : public MicroAccountWRH {
private:
    std::string wxNo_;      // 独立微信号
    std::string boundQQ_;   // 绑定的QQ主号；空串表示未绑定

public:
    WeChatAccountWRH(UserWRH* owner, const MyDateWRH& apply);

    const char* svcName()    const override { return "微信"; }
    bool        isSharedID() const override { return false; }
    bool        isWeChat()   const override { return true; }
    std::string svcNo()      const override;   // 返回独立微信号
    void        show()       const override;   // 追加显示绑定状态

    // ---- 独立号 + QQ绑定 ----
    const std::string& wxNo() const { return wxNo_; }
    void setWxNo(const std::string& w) { wxNo_ = w; }

    bool hasBoundQQ() const { return !boundQQ_.empty(); }
    const std::string& boundQQ() const { return boundQQ_; }
    void bindQQ(const std::string& qq) { boundQQ_ = qq; }
    void unbindQQ() { boundQQ_.clear(); }
};

#endif
