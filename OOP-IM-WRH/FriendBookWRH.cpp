// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：FriendBookWRH.cpp
#include "FriendBookWRH.h"
#include "UserWRH.h"
#include <algorithm>
#include <iostream>

// =====================================================================
// FriendBookWRH.cpp  好友信息容器类实现
// =====================================================================

bool FriendBookWRH::add(UserWRH* person, const std::string& remark) {
    if (!person) return false;
    if (has(person)) return false;          // 已存在则不能重复添加
    v_.push_back(FriendInfoWRH(person, remark));
    return true;
}

bool FriendBookWRH::remove(UserWRH* person) {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].person() == person) {
            v_.erase(v_.begin() + i);
            return true;
        }
    return false;
}

bool FriendBookWRH::has(UserWRH* person) const {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].person() == person) return true;
    return false;
}

FriendInfoWRH* FriendBookWRH::find(UserWRH* person) {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].person() == person) return &v_[i];
    return nullptr;
}

bool FriendBookWRH::setRemark(UserWRH* person, const std::string& r) {
    FriendInfoWRH* p = find(person);
    if (!p) return false;
    p->setRemark(r);
    return true;
}

// 列好友：由于好友记录只保存指针，展示需要 UserWRH 的完整定义
void FriendBookWRH::list() const {
    for (size_t i = 0; i < v_.size(); ++i) {
        const FriendInfoWRH& f = v_[i];
        std::cout << "  " << (i + 1) << ". 昵称[" << f.person()->nick()
                  << "] 备注[" << f.remark() << "]\n";
    }
}

void FriendBookWRH::listSimple() const {
    for (size_t i = 0; i < v_.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << v_[i].person()->nick() << "\n";
    }
}

std::vector<UserWRH*> FriendBookWRH::commonWith(const FriendBookWRH& o) const {
    std::vector<UserWRH*> r;
    for (size_t i = 0; i < v_.size(); ++i)
        if (o.has(v_[i].person()))        // 对方也有此人 -> 共同好友
            r.push_back(v_[i].person());
    return r;
}
