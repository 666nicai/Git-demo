// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：PlatformWRH.h
#ifndef PLATFORM_WRH_H
#define PLATFORM_WRH_H

#include "ConstWRH.h"
#include "MyDateWRH.h"
#include <string>
#include <vector>

class UserWRH;
class MicroAccountWRH;
class GroupWRH;

// =====================================================================
// PlatformWRH  腾讯立体社交平台总控类
// ---------------------------------------------------------------------
// 管理所有平台用户(users_)与所有微X群(groups_)，负责：
//   1) 演示数据初始化 / 文件I/O持久化(实例化读入、析构写回)
//   2) 数字化主菜单(可逐级返回上级)
//   3) 用户、好友、群的各类业务操作总调度
// =====================================================================
class PlatformWRH {
private:
    std::vector<UserWRH*>    users_;   // 全部平台用户(自然人)
    std::vector<GroupWRH*>   groups_;  // 全部微X群(QQ群+微信群)
    std::string              file_;    // 持久化文件名
    int                      tempNo_;  // 临时讨论组群号分配起点

    // ---- 内部工具 ----
    UserWRH*         findUser(const std::string& id) const;
    UserWRH*         pickUser(const char* tip) const;   // 让用户从列表选一个
    void             listAllUsersShort() const;
    GroupWRH*        pickGroup(const char* tip) const;  // 让用户从列表选一个群
    MicroType        pickService() const;               // 1=QQ 2=微博 3=微信
    // 列出该用户已开通的服务，让用户选其一
    MicroType        pickServiceFromUser(const UserWRH* u) const;
    static std::string toSvcName(MicroType t);
    static bool      parseSvc(const std::string& s, MicroType& t);

    // ---- 各业务菜单 ----
    void menuUser();
    void menuFriend();
    void menuGroup();
    void menuFile();
    void menuOverview();

    // ---- 关键业务动作 ----
    void demoOpenServices();                 // 开通管理演示
    void demoLogin();                        // 登录管理(一个登录全登录)
    void demoFriendCRUD();                   // 好友增删改查
    void demoCommonFriend();                 // 微X间共同好友
    void demoWeChatRecFriend();              // 微信加QQ推荐好友
    void demoGroups();                       // 群管理全流程 + 多态差异
    void demoModeSwitch();                   // 管理模式动态切换(数据不受伤)
public:
    explicit PlatformWRH(const std::string& file = "platform_data.txt");
    ~PlatformWRH();                          // 析构时把容器写回文件

    void buildDemo();                        // 固化演示数据
    bool load();                             // 实例化时读入文件
    void save() const;                       // 写回文件

    void run();                              // 数字化主菜单
    void autoDemo();                         // 一条龙自动演示

    int userCount() const { return (int)users_.size(); }
    int groupCount() const { return (int)groups_.size(); }
};

#endif
