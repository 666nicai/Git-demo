// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：UserWRH.h
#ifndef USER_WRH_H
#define USER_WRH_H

#include "BaseInfoWRH.h"
#include "ConstWRH.h"
#include "MyDateWRH.h"
#include <set>
#include <string>
#include <vector>

class MicroAccountWRH;
class WeChatAccountWRH;

// =====================================================================
// UserWRH  平台用户类（自然人）
// ---------------------------------------------------------------------
// 一个自然人可以开通多个微X服务。其基本信息(昵称/出生/所在地/主号申请时间)
// 通过继承复用抽象基础类 BaseInfoWRH。
//     - 主号 mainNo_：QQ与微博共享的ID
//     - 微信号 wxNo_ ：微信的独立ID（可与QQ绑定）
//     - 开通管理：accs_ 存放已开通的各微X服务账号对象
//     - 登录管理：任一个服务登录，其它已开通服务自动登录
// =====================================================================
class UserWRH : public BaseInfoWRH {
private:
    std::string mainNo_;                              // 主号：QQ/微博共享
    std::string wxNo_;                                // 微信号（微信独立ID）
    std::vector<MicroAccountWRH*> accs_;              // 已开通的微X服务账号
public:
    UserWRH(const std::string& id, const std::string& nick,
            const MyDateWRH& birth, const MyDateWRH& apply,
            const std::string& city);

    virtual ~UserWRH();

    // ---- ID ----
    const std::string& mainNo() const { return mainNo_; }   // QQ/微博共享主号
    const std::string& wxNo()   const { return wxNo_; }

    // ---- 开通管理 ----
    MicroAccountWRH* openService(MicroType t);   // 开通一个微X服务（可多次）
    void  closeService(MicroType t);             // 注销（关闭）一个服务
    bool  hasService(MicroType t) const;
    MicroAccountWRH* account(MicroType t) const; // 找已开通的某服务账号
    const std::vector<MicroAccountWRH*>& accounts() const { return accs_; }
    int   openedCount() const { return (int)accs_.size(); }
    // 已开通服务集合（便于显示；.cpp 实现）
    std::set<MicroType> openedSet() const;

    // ---- 登录管理 ----
    bool login(MicroType t);   // 某服务登录 → 其它已开通服务自动登录
    void logoutAll();
    bool anyLogged() const;

    // ---- 微信独立ID 与 QQ绑定 ----
    void setWxNo(const std::string& w) { wxNo_ = w; }
    WeChatAccountWRH* wxAccount() const;   // 返回微信号账号指针（未开通返回null）
    void bindWxToQQ(const std::string& qq);   // 微信绑定QQ

    // ---- 展示 ----
    void showProfile() const;    // 基本信息(含主号/微信号/开通服务)
    void showOpened()   const;   // 每个开通的服务账号详情
};

#endif
