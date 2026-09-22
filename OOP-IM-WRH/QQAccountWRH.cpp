// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：QQAccountWRH.cpp
#include "QQAccountWRH.h"
#include "UserWRH.h"

// QQ 号码 = 平台主号（与微博共享）
std::string QQAccountWRH::svcNo() const {
    return owner_->mainNo();
}
