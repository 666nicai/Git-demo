// ============================================================
// 文件名: PlatformWRH.h
// 描述  : 平台总控类——用户、群、文件读写、菜单与自动演示
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef PLATFORM_WRH_H
#define PLATFORM_WRH_H

#include <bits/stdc++.h>

#include "ConstWRH.h"

class UserWRH;
class MicroAccountWRH;
class GroupWRH;

// 腾讯立体社交平台总控类。
// 管两件事：
//   1) 数据：users_ 所有用户，groups_ 所有群，file_ 持久化文件
//   2) 交互：数字化主菜单、各级子菜单、一条龙自动演示
// 数据文件在构造时读入、析构时写回；如果文件不存在，就建一套演示数据。
class PlatformWRH {
private:
    std::vector<UserWRH*>  users_;    // 全部用户
    std::vector<GroupWRH*> groups_;   // 全部群（QQ群 + 微信群）
    std::string file_;                // 持久化文件名
    int tempNo_;                      // 临时讨论组群号：从已有最大群号往后排
    bool autoSave_;                   // 析构时是否写回（只读验证实例设 false）

    // ---- 内部工具 ----
    void      clearAll();                               // 清空并释放全部对象
    UserWRH*  findUser(const std::string& id) const;
    UserWRH*  pickUser(const char* tip) const;
    void      listAllUsersShort() const;
    GroupWRH* pickGroup(const char* tip) const;
    MicroType pickService() const;                      // 1=QQ 2=微博 3=微信
    MicroType pickServiceFromUser(const UserWRH* u) const;
    static std::string toSvcName(MicroType t);
    static bool        parseSvc(const std::string& s, MicroType& t);

    // 从 from 服务的好友里，找出“已开通 to 服务、但还不是 to 好友”的人
    std::vector<UserWRH*> recommendFriends(const UserWRH* op,
                                           MicroType from, MicroType to) const;

    // 注销某服务后，把该用户从对应服务的群里清掉；群主先转让，空群删除
    void cleanGroupsAfterClose(UserWRH* u, MicroType t);

    // ---- 各级菜单 ----
    void menuUser();
    void menuFriend();
    void menuGroup();
    void menuFile();
    void menuOverview();

    // ---- 自动演示的各段 ----
    void demoOpenServices();     // 开通/注销
    void demoLogin();            // 一个服务登录，其它自动登录
    void demoFriendCRUD();       // 好友增删改查
    void demoCommonFriend();     // 两个服务的共同好友
    void demoWeChatRecFriend();  // 微信添加 QQ 推荐好友
    void demoGroups();           // 群管理全流程 + 多态差异
    void demoModeSwitch();       // 管理模式动态切换，成员数据不受伤

public:
    // autoSave = false 的实例只读文件，析构时不要覆盖原文件
    explicit PlatformWRH(const std::string& file = "platform_data.txt",
                         bool autoSave = true);
    ~PlatformWRH();

    // 平台对象拥有 users_/groups_ 里的对象，复制会 double free
    PlatformWRH(const PlatformWRH&) = delete;
    PlatformWRH& operator=(const PlatformWRH&) = delete;

    void buildDemo();     // 建一套固定的演示数据
    bool load();          // 从文件读入；文件不存在或没有用户时返回 false
    void save() const;    // 把当前数据写回文件

    void run();           // 数字化主菜单
    void autoDemo();      // 一条龙自动演示
    void showBrief() const;   // 精简总览（文件读回验证用）

    int userCount()  const { return (int)users_.size(); }
    int groupCount() const { return (int)groups_.size(); }
};

#endif
