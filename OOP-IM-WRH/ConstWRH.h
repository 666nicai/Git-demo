#ifndef CONST_WRH_H
#define CONST_WRH_H

#include <string>
#include <set>
#include <sstream>

// =====================================================================
// ConstWRH.h  全局常量与公共小工具（非类，仅常量/内联函数）
// 工程：模拟即时通信系统（腾讯立体社交平台）
// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// =====================================================================

// ---- 微X 服务类型（可继续扩展：微商、微唱、微走、微笑…）----
enum MicroType {
    MQQ = 0,       // QQ
    MWB = 1,       // 微博（与QQ共享ID）
    MWX = 2,       // 微信（独立ID，可绑定QQ）
    MCOUNT = 3
};

// ---- 群成员角色 ----
enum MemberRole {
    ROLE_OWNER = 2,   // 群主
    ROLE_ADMIN = 1,   // 管理员（QQ群有，微信群无）
    ROLE_MEMBER = 0   // 普通成员
};

// ---- 微X 名字 ----
inline const char* microName(MicroType t) {
    switch (t) {
        case MQQ: return "QQ";
        case MWB: return "微博";
        case MWX: return "微信";
        default:  return "未知";
    }
}

// ---- 角色名字 ----
inline const char* roleName(int r) {
    switch (r) {
        case ROLE_OWNER: return "群主";
        case ROLE_ADMIN: return "管理员";
        default:         return "成员";
    }
}

// ---- 开通集合 -> 可读串（例：QQ、微博、微信）----
inline std::string openedToStr(const std::set<MicroType>& s) {
    std::string r;
    for (int i = 0; i < MCOUNT; ++i)
        if (s.count((MicroType)i)) {
            if (!r.empty()) r += "、";
            r += microName((MicroType)i);
        }
    if (r.empty()) r = "（未开通）";
    return r;
}

#endif
