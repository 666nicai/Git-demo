// ============================================================
// 文件名: FriendBookWRH.h
// 描述  : 好友容器——某个服务账号下的全部好友记录
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef FRIENDBOOK_WRH_H
#define FRIENDBOOK_WRH_H

#include <bits/stdc++.h>

#include "FriendInfoWRH.h"

class UserWRH;   // 前向声明，具体实现放到 .cpp 里再包含 UserWRH.h

// 每个微X服务账号各有一个好友容器：
//   QQ账号一个、微博账号一个、微信账号一个……
// 这样“QQ好友”和“微信好友”天然分开，才能演示共同好友与跨服务推荐。
class FriendBookWRH {
private:
    std::vector<FriendInfoWRH> v_;   // 好友记录集合

public:
    FriendBookWRH() {}

    // ---- 增删改查 ----
    bool add(UserWRH* person, const std::string& remark);   // 添加（自动去重）
    bool remove(UserWRH* person);                           // 删除
    bool has(UserWRH* person) const;                        // 是否已是好友
    FriendInfoWRH* find(UserWRH* person);                   // 查单条，找不到返回空
    bool setRemark(UserWRH* person, const std::string& r);  // 改备注
    int  count() const { return (int)v_.size(); }

    // ---- 展示 ----
    void list() const;         // 列出全部好友（含备注）
    void listSimple() const;   // 只列昵称

    // 求两个服务好友列表的“共同好友”
    std::vector<UserWRH*> commonWith(const FriendBookWRH& o) const;

    const std::vector<FriendInfoWRH>& all() const { return v_; }
};

#endif
