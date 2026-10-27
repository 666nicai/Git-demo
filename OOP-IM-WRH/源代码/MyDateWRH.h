// ============================================================
// 文件名: MyDateWRH.h
// 描述  : 日期类——出生日期、T龄申请日期的表示与天数计算
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#ifndef MYDATE_WRH_H
#define MYDATE_WRH_H

#include <bits/stdc++.h>


// 日期类：只做三件事——存年月日、算相差天数、和字符串互转。
// 不直接上 <chrono>，是为了让日期计算过程在课设里一眼能看懂。
class MyDateWRH {
public:
    int y, m, d;   // 年、月、日；类很小，直接公开，读写方便

    MyDateWRH(int yy = 2000, int mm = 1, int dd = 1)
        : y(yy), m(mm), d(dd) {}

    static MyDateWRH today();              // 取系统当前日期
    long days() const;                     // 距 1970-01-01 的天数
    long diff(const MyDateWRH& o) const {  // this - o，单位：天
        return days() - o.days();
    }

    std::string str() const;               // 转成 "YYYY-MM-DD"
    bool parse(const std::string& s);      // 解析 "YYYY-MM-DD"，非法返回 false
};

#endif
