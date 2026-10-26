// ============================================================
// 文件名: QQAccountWRH.cpp
// 描述  : QQ账号实现——号码 = 平台主号
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "QQAccountWRH.h"
#include "UserWRH.h"

// QQ 号码 = 平台主号（与微博是同一个号）
std::string QQAccountWRH::svcNo() const {
    return owner_->mainNo();
}
