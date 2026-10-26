// ============================================================
// 文件名: WeiboAccountWRH.cpp
// 描述  : 微博账号实现——号码 = 平台主号
// 作者  : 魏荣赫  学号 55250124  软件一班(552401班)
// ============================================================
#include "WeiboAccountWRH.h"
#include "ShortTypes.h"
#include "UserWRH.h"

// 微博号码 = 平台主号（与QQ相同）
std::string WeiboAccount::svcNo() const {
    return owner_->mainNo();
}
