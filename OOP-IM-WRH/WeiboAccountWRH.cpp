// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：WeiboAccountWRH.cpp
#include "WeiboAccountWRH.h"
#include "UserWRH.h"

// 微博与QQ共享ID：微博号码 = 平台主号（与QQ相同）
std::string WeiboAccountWRH::svcNo() const {
    return owner_->mainNo();
}
