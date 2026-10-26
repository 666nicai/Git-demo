// ============================================================
// 文件名: UserWRH.h
// 描述  : 平台用户类（自然人）——主号、微信号、开通列表、登录
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#ifndef USER_WRH_H
#define USER_WRH_H

#include "BaseInfoWRH.h"
#include "ConstWRH.h"
#include <set>
#include <string>
#include <vector>

class MicroAccountWRH;
class WeChatAccountWRH;

// 平台用户（自然人）。
// 基本信息通过继承 BaseInfoWRH 复用：昵称 / 出生 / 申请时间 / 所在地。
//   mainNo_：QQ 与微博共享的主号
//   wxNo_  ：微信独立号（可与QQ绑定）
//   accs_  ：已经开通的服务账号，开一个就多一个对象
// 登录规则：任意一个服务登录，其它已开通服务全部自动登录。
class UserWRH : public BaseInfoWRH {
private:
    std::string mainNo_;                  // 主号：QQ/微博共享
    std::string wxNo_;                    // 微信号（微信独立ID）
    std::vector<MicroAccountWRH*> accs_;  // 已开通的服务账号

public:
    UserWRH(const std::string& id, const std::string& nick,
            const MyDateWRH& birth, const MyDateWRH& apply,
            const std::string& city);
    virtual ~UserWRH();

    // 用户对象拥有 accs_ 里的账号，复制会导致两次 delete，直接禁止
    UserWRH(const UserWRH&) = delete;
    UserWRH& operator=(const UserWRH&) = delete;

    // ---- ID ----
    const std::string& mainNo() const { return mainNo_; }
    const std::string& wxNo()   const { return wxNo_; }

    // ---- 开通管理 ----
    // quiet = true 时不在控制台打印“开通成功”，读文件恢复数据时用
    MicroAccountWRH* openService(MicroType t, bool quiet = false);
    void             closeService(MicroType t);
    bool             hasService(MicroType t) const;
    MicroAccountWRH* account(MicroType t) const;
    const std::vector<MicroAccountWRH*>& accounts() const { return accs_; }
    int              openedCount() const { return (int)accs_.size(); }
    std::set<MicroType> openedSet() const;

    // ---- 登录管理 ----
    bool login(MicroType t);   // 一个服务登录 → 其它已开通服务自动登录
    void logoutAll();
    bool anyLogged() const;

    // ---- 微信独立ID 与 QQ绑定 ----
    void setWxNo(const std::string& w) { wxNo_ = w; }
    WeChatAccountWRH* wxAccount() const;
    void bindWxToQQ(const std::string& qq, bool quiet = false);

    // ---- 展示 ----
    void showProfile() const;   // 基本资料 + 主号/微信号 + 开通状态
    void showOpened()  const;   // 每个服务账号的详情（多态）
};

#endif
