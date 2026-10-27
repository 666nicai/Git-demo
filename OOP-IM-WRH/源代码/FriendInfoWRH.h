// ============================================================
// 文件名: FriendInfoWRH.h
// 描述  : 好友记录——好友本人 + 我给他的备注
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef FRIENDINFO_WRH_H
#define FRIENDINFO_WRH_H

#include <bits/stdc++.h>


class UserWRH;   // 前向声明：这里只存指针，不需要 UserWRH 的完整定义

// 一条好友记录。
// 备注是“我”给“他”的，所以 A 看 B 和 B 看 A 的备注可以不一样。
class FriendInfoWRH {
private:
    UserWRH*    person_;   // 指向好友本人
    std::string remark_;   // 我对他的备注，可以为空

public:
    FriendInfoWRH(UserWRH* p = nullptr, const std::string& r = "")
        : person_(p), remark_(r) {}

    UserWRH* person() const { return person_; }
    void setPerson(UserWRH* p) { person_ = p; }

    const std::string& remark() const { return remark_; }
    void setRemark(const std::string& r) { remark_ = r; }

    // 好友记录只按“人”比较，备注不参与
    bool operator==(const FriendInfoWRH& o) const { return person_ == o.person_; }
};

#endif
