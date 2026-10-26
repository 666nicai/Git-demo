// ============================================================
// 文件名: GroupMemberBookWRH.cpp
// 描述  : 群成员容器实现——增删查、改角色、展示
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "GroupMemberBookWRH.h"
#include "UserWRH.h"
#include <iostream>

using namespace std;

bool GroupMemberBookWRH::add(UserWRH* u, MemberRole r) {
    if (!u || isMember(u)) return false;
    v_.push_back(GroupMemberWRH(u, r));
    return true;
}

bool GroupMemberBookWRH::remove(UserWRH* u) {
    for (size_t i = 0; i < v_.size(); ++i) {
        if (v_[i].user() != u) continue;
        v_.erase(v_.begin() + i);
        return true;
    }
    return false;
}

bool GroupMemberBookWRH::isMember(UserWRH* u) const {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].user() == u) return true;
    return false;
}

GroupMemberWRH* GroupMemberBookWRH::find(UserWRH* u) {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].user() == u) return &v_[i];
    return nullptr;
}

const GroupMemberWRH* GroupMemberBookWRH::find(UserWRH* u) const {
    for (size_t i = 0; i < v_.size(); ++i)
        if (v_[i].user() == u) return &v_[i];
    return nullptr;
}

bool GroupMemberBookWRH::setRole(UserWRH* u, MemberRole r) {
    GroupMemberWRH* m = find(u);
    if (!m) return false;
    m->setRole(r);
    return true;
}

vector<UserWRH*> GroupMemberBookWRH::users() const {
    vector<UserWRH*> r;
    for (size_t i = 0; i < v_.size(); ++i)
        r.push_back(v_[i].user());
    return r;
}

void GroupMemberBookWRH::list() const {
    if (v_.empty()) {
        cout << "    （群内暂无成员）\n";
        return;
    }
    for (size_t i = 0; i < v_.size(); ++i) {
        const GroupMemberWRH& m = v_[i];
        cout << "    " << (i + 1) << ". " << m.user()->nick()
             << "（" << m.user()->mainNo() << "）  —— "
             << roleName((int)m.role()) << "\n";
    }
}
