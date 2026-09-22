// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：BaseInfoWRH.cpp
#include "BaseInfoWRH.h"

// =====================================================================
// BaseInfoWRH.cpp  抽象基础信息类的实现
// 昵称/出生日期/号码申请时间(T龄)/所在地 —— 供 UserWRH 等派生复用
// =====================================================================

BaseInfoWRH::BaseInfoWRH(const std::string& nick,
                         const MyDateWRH& birth,
                         const MyDateWRH& applyDate,
                         const std::string& city)
    : nick_(nick), birth_(birth), applyDate_(applyDate), city_(city) {}

// T龄 = 今天 - 申请时间（天数，0 表示当天申请）
long BaseInfoWRH::tAge() const {
    return MyDateWRH::today().diff(applyDate_);
}

void BaseInfoWRH::show() const {
    std::cout << "昵称      : " << nick_ << "\n";
    std::cout << "出生日期  : " << birth_.str() << "\n";
    std::cout << "所在地    : " << city_ << "\n";
    std::cout << "号码申请  : " << applyDate_.str()
              << "（T龄 " << tAge() << " 天）\n";
}
