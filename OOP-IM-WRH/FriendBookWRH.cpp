// ============================================================
// 文件名: FriendBookWRH.cpp
// 描述  : 好友容器实现——增删改查、展示、共同好友
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "FriendBookWRH.h"
#include "UserWRH.h"
#include <iostream>

using namespace std;

bool FriendBookWRH::add(UserWRH* person, const string& remark) {
    if (!person) return false;
    if (has(person)) return false;          // 已经在列表里，不重复加
    v_.push_back(FriendInfoWRH(person, remark));
    return true;
}

bool FriendBookWRH::remove(UserWRH* person) {
    for (size_t i = 0; i < v_.size(); ++i) {
        if (v_[i].person() != person) continue;
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

bool FriendBookWRH::setRemark(UserWRH* person, const string& r) {
    FriendInfoWRH* p = find(person);
    if (!p) return false;
    p->setRemark(r);
    return true;
}

void FriendBookWRH::list() const {
    if (v_.empty()) {
        cout << "    （暂无好友）\n";
        return;
    }
    for (size_t i = 0; i < v_.size(); ++i) {
        const FriendInfoWRH& f = v_[i];
        cout << "    " << (i + 1) << ". 昵称[" << f.person()->nick()
             << "] 主号[" << f.person()->mainNo()
             << "] 备注[" << f.remark() << "]\n";
    }
}

void FriendBookWRH::listSimple() const {
    if (v_.empty()) {
        cout << "（暂无好友）\n";
        return;
    }
    for (size_t i = 0; i < v_.size(); ++i)
        cout << (i ? "、" : "") << v_[i].person()->nick();
    cout << "\n";
}

// 共同好友：遍历自己的列表，看对方列表里有没有同一个人
vector<UserWRH*> FriendBookWRH::commonWith(const FriendBookWRH& o) const {
    vector<UserWRH*> r;
    for (size_t i = 0; i < v_.size(); ++i)
        if (o.has(v_[i].person()))
            r.push_back(v_[i].person());
    return r;
}
