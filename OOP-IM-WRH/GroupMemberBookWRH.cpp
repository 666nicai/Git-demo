// ============================================================
// 文件名: GroupMemberBookWRH.cpp
// 描述  : 群成员容器实现——增删查、改角色、展示
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "GroupMemberBookWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"
#include <iostream>

using namespace std;

bool MemberBook::add(User* u, MemberRole r) {
    if (!u || isMember(u)) return false;
    v_.push_back(GroupMember(u, r));
    return true;
}

bool MemberBook::remove(User* u) {
    for (size_t i = 0; i < v_.size(); ++i) {
        if (v_[i].user() != u) continue;
        v_.erase(v_.begin() + i);
        return true;
    }
    return false;
}

bool MemberBook::isMember(User* u) const {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].user() == u) return true;
    return false;
}

GroupMember* MemberBook::find(User* u) {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].user() == u) return &v_[i];
    return nullptr;
}

const GroupMember* MemberBook::find(User* u) const {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].user() == u) return &v_[i];
    return nullptr;
}

bool MemberBook::setRole(User* u, MemberRole r) {
    GroupMember* m = find(u);
    if (!m) return false;
    m->setRole(r);
    return true;
}

vector<User*> MemberBook::users() const {
    vector<User*> r;
    for (size_t i = 0; i < v_.size(); ++i)
        r.push_back(v_[i].user());
    return r;
}

void MemberBook::list() const {
    if (v_.empty()) {
        cout << "    （群内暂无成员）\n";
        return;
    }
    for (size_t i = 0; i < v_.size(); ++i) {
        const GroupMember& m = v_[i];
        cout << "    " << (i + 1) << ". " << m.user()->nick()
             << "（" << m.user()->mainNo() << "）  —— "
             << roleName((int)m.role()) << "\n";
    }
}
