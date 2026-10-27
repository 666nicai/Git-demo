// ============================================================
// 文件名: BaseInfoWRH.cpp
// 描述  : 抽象基础信息类实现——公共资料的构造、T龄与显示
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#include "BaseInfoWRH.h"
#include "ShortTypes.h"
#include <bits/stdc++.h>

using namespace std;

BaseInfoWRH::BaseInfoWRH(const string& nick,
                         const Date& birth,
                         const Date& applyDate,
                         const string& city)
    : nick_(nick), birth_(birth), applyDate_(applyDate), city_(city) {}

// T龄 = 今天 - 申请日期（天）；当天申请就是 0 天
long BaseInfo::tAge() const {
    return Date::today().diff(applyDate_);
}

void BaseInfo::show() const {
    cout << "昵称      : " << nick_ << "\n";
    cout << "出生日期  : " << birth_.str() << "\n";
    cout << "所在地    : " << city_ << "\n";
    cout << "号码申请  : " << applyDate_.str()
         << "（T龄 " << tAge() << " 天）\n";
}
