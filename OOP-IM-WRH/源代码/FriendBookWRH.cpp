// ============================================================
// 文件名: FriendBookWRH.cpp
// 描述  : 好友容器实现——增删改查、展示、共同好友
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#include "FriendBookWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"
#include <bits/stdc++.h>

using namespace std;

bool FriendBook::add(User* person, const string& remark) {
    if (!person) return false;
    if (has(person)) return false;          // 已经在列表里，不重复加
    v_.push_back(FriendInfo(person, remark));
    return true;
}

bool FriendBook::remove(User* person) {
    for (size_t i = 0; i < v_.size(); ++i) {
        if (v_[i].person() != person) continue;
        v_.erase(v_.begin() + i);
        return true;
    }
    return false;
}

bool FriendBook::has(User* person) const {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].person() == person) return true;
    return false;
}

FriendInfo* FriendBook::find(User* person) {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].person() == person) return &v_[i];
    return nullptr;
}

bool FriendBook::setRemark(User* person, const string& r) {
    FriendInfo* p = find(person);
    if (!p) return false;
    p->setRemark(r);
    return true;
}

void FriendBook::list() const {
    if (v_.empty()) {
        cout << "    （暂无好友）\n";
        return;
    }
    for (size_t i = 0; i < v_.size(); ++i) {
        const FriendInfo& f = v_[i];
        cout << "    " << (i + 1) << ". 昵称[" << f.person()->nick()
             << "] 主号[" << f.person()->mainNo()
             << "] 备注[" << f.remark() << "]\n";
    }
}

void FriendBook::listSimple() const {
    if (v_.empty()) {
        cout << "（暂无好友）\n";
        return;
    }
    for (size_t i = 0; i < v_.size(); ++i)
        cout << (i ? "、" : "") << v_[i].person()->nick();
    cout << "\n";
}

// 共同好友：遍历自己的列表，看对方列表里有没有同一个人
vector<User*> FriendBook::commonWith(const FriendBook& o) const {
    vector<User*> r;
    for (size_t i = 0; i < v_.size(); ++i)
        if (o.has(v_[i].person()))
            r.push_back(v_[i].person());
    return r;
}
