// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：FriendBookWRH.h
#ifndef FRIENDBOOK_WRH_H
#define FRIENDBOOK_WRH_H

#include "FriendInfoWRH.h"
#include <vector>
#include <string>

class UserWRH;  // 前向声明，.cpp 中才需要完整定义

// =====================================================================
// FriendBookWRH  好友信息容器类
// 每个微X服务账号对象各自拥有一个好友容器（如"QQ账号的好友列表"、
// "微信账号的好友列表"），对应题目第2点"微X之间各自共同好友"
// =====================================================================
class FriendBookWRH {
private:
    std::vector<FriendInfoWRH> v_;   // 好友记录集合
public:
    FriendBookWRH() {}

    // ---- 增删改查 ----
    bool add(UserWRH* person, const std::string& remark);   // 添加（去重）
    bool remove(UserWRH* person);                           // 删除
    bool has(UserWRH* person) const;                        // 是否已是好友
    FriendInfoWRH* find(UserWRH* person);                   // 查询单条
    bool setRemark(UserWRH* person, const std::string& r);  // 修改备注
    int  count() const { return (int)v_.size(); }

    // ---- 展示 ----
    void list() const;                          // 列出全部好友(含备注)
    void listSimple() const;                    // 精简列表(仅昵称)

    // 求本容器与另一服务好友容器的"共同好友"（返回好友指针集合）
    std::vector<UserWRH*> commonWith(const FriendBookWRH& o) const;

    const std::vector<FriendInfoWRH>& all() const { return v_; }
};

#endif
