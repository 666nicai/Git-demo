// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：FriendInfoWRH.h
#ifndef FRIENDINFO_WRH_H
#define FRIENDINFO_WRH_H

#include <string>

class UserWRH;   // 前向声明，避免与 UserWRH.h 循环包含

// =====================================================================
// FriendInfoWRH  好友信息类
// 一条好友记录：指向好友本人(UserWRH*) + 我给他的备注
// 好友列表按"微X服务"划分，存放在对应服务的账号对象内(见 FriendBookWRH)
// =====================================================================
class FriendInfoWRH {
private:
    UserWRH*   person_;   // 好友（对应的自然人/用户）
    std::string remark_;  // 我对他的备注
public:
    FriendInfoWRH(UserWRH* p = nullptr, const std::string& r = "")
        : person_(p), remark_(r) {}

    UserWRH* person() const { return person_; }
    void setPerson(UserWRH* p) { person_ = p; }

    const std::string& remark() const { return remark_; }
    void setRemark(const std::string& r) { remark_ = r; }

    bool operator==(const FriendInfoWRH& o) const { return person_ == o.person_; }
};

#endif
