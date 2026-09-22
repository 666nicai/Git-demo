// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：BaseInfoWRH.h
#ifndef BASEINFO_WRH_H
#define BASEINFO_WRH_H

#include "MyDateWRH.h"
#include <string>
#include <iostream>

// =====================================================================
// BaseInfoWRH  抽象基础信息类（抽象、被其它功能复用）
// 归纳各“信息”类共有的字段：昵称 / 出生时间 / T龄(号码申请时间) / 所在地
// 提供虚拟接口 show()，由派生类(UserWRH 等)多态重写
// =====================================================================
class BaseInfoWRH {
protected:
    std::string nick_;        // 昵称
    MyDateWRH   birth_;       // 出生时间
    MyDateWRH   applyDate_;   // 号码申请时间(T龄)
    std::string city_;        // 所在地
public:
    BaseInfoWRH(const std::string& nick = "未命名",
                const MyDateWRH& birth = MyDateWRH(2000, 1, 1),
                const MyDateWRH& applyDate = MyDateWRH(2020, 1, 1),
                const std::string& city = "长春");
    virtual ~BaseInfoWRH() {}

    // ---- 只读接口 ----
    const std::string& nick()      const { return nick_; }
    const MyDateWRH&   birth()     const { return birth_; }
    const MyDateWRH&   applyDate() const { return applyDate_; }
    const std::string& city()      const { return city_; }

    // T龄：今天 - 申请时间（天数）
    long tAge() const;

    // ---- 修改接口 ----
    void setNick(const std::string& n) { nick_ = n; }
    void setBirth(const MyDateWRH& b)  { birth_ = b; }
    void setCity(const std::string& c) { city_ = c; }

    // 多态显示接口（基础信息部分）
    virtual void show() const;
};

#endif
