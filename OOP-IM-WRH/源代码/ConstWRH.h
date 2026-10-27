// ============================================================
// 文件名: ConstWRH.h
// 描述  : 全局枚举与公共小工具——服务类型、成员角色、名字转换
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef CONST_WRH_H
#define CONST_WRH_H

#include <bits/stdc++.h>


// ==================== 微X 服务类型 ====================
// 以后要加“微商 / 微唱 / 微走”，在这里加一项，再把 MCOUNT 往后挪即可。
enum MicroType {
    MQQ    = 0,   // QQ：与微博共享主号
    MWB    = 1,   // 微博：与QQ共享主号
    MWX    = 2,   // 微信：独立号码，可以绑定QQ
    MCOUNT = 3    // 服务总数；同时当“无效值”用，方便循环和判断
};

// ==================== 群成员角色 ====================
enum MemberRole {
    ROLE_OWNER  = 2,   // 群主
    ROLE_ADMIN  = 1,   // 管理员（只有QQ群有）
    ROLE_MEMBER = 0    // 普通成员
};

// 枚举 -> 中文名
inline const char* microName(MicroType t) {
    switch (t) {
        case MQQ: return "QQ";
        case MWB: return "微博";
        case MWX: return "微信";
        default:  return "未知";
    }
}

// 角色 -> 中文名
inline const char* roleName(int r) {
    switch (r) {
        case ROLE_OWNER: return "群主";
        case ROLE_ADMIN: return "管理员";
        default:         return "成员";
    }
}

// 把“已开通服务集合”拼成一行中文，例如：QQ、微博、微信
inline std::string openedToStr(const std::set<MicroType>& s) {
    std::string r;
    for (int i = 0; i < MCOUNT; ++i) {
        if (!s.count((MicroType)i)) continue;
        if (!r.empty()) r += "、";
        r += microName((MicroType)i);
    }
    if (r.empty()) r = "（未开通）";
    return r;
}

#endif
