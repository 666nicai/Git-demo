// ============================================================
// 文件名: BaseInfoWRH.h
// 描述  : 抽象基础信息类——昵称/出生日期/申请日期(T龄)/所在地
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef BASEINFO_WRH_H
#define BASEINFO_WRH_H

#include <bits/stdc++.h>

#include "MyDateWRH.h"

// 抽象基础信息类：把“人”和“账号”都共有的资料抽出来，派生类直接复用。
// show() 是虚函数，派生类可以在这段资料后面继续补充自己的内容。
class BaseInfoWRH {
protected:
    std::string nick_;       // 昵称
    MyDateWRH   birth_;      // 出生日期
    MyDateWRH   applyDate_;  // 号码申请日期，用来算 T 龄
    std::string city_;       // 所在地

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

    // T龄 = 今天 - 申请日期（天）
    long tAge() const;

    // 多态显示：先由基类打印公共资料，派生类可以重写后再追加
    virtual void show() const;

    // ---- 修改接口 ----
    void setNick(const std::string& n) { nick_ = n; }
    void setBirth(const MyDateWRH& b)  { birth_ = b; }
    void setCity(const std::string& c) { city_ = c; }
};

#endif
