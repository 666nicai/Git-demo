// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：MyDateWRH.h
#ifndef MYDATE_WRH_H
#define MYDATE_WRH_H

#include <string>

// 日期类：用于出生日期、T龄申请日期的表示与计算
class MyDateWRH {
public:
    int y, m, d;
    MyDateWRH(int yy = 2000, int mm = 1, int dd = 1) : y(yy), m(mm), d(dd) {}
    static MyDateWRH today();
    // 返回该日期距 epoch 的天数（用于差计算）
    long days() const;
    // 两个日期相差天数：this - o
    long diff(const MyDateWRH& o) const { return days() - o.days(); }
    std::string str() const;          // YYYY-MM-DD
    bool parse(const std::string& s); // 解析 YYYY-MM-DD
};

#endif
