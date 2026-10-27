// ============================================================
// 文件名: MyDateWRH.cpp
// 描述  : 日期类实现——公历天数换算与日期合法性检查
// 作者  : 魏荣赫  学号 55250124  软件一班(552501班)
// ============================================================
#include "MyDateWRH.h"
#include "ShortTypes.h"
#include <bits/stdc++.h>

using namespace std;

// 判断闰年：四年一闰、百年不闰、四百年再闰
static bool isLeap(int y) {
    return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
}

// 某年某月有几天；调用前保证 m 在 1~12
static int daysInMonth(int y, int m) {
    static const int tab[13] = {0, 31, 28, 31, 30, 31, 30,
                                31, 31, 30, 31, 30, 31};
    if (m == 2 && isLeap(y)) return 29;
    return tab[m];
}

// Howard Hinnant 的 civil 算法：把年月日换成距 1970-01-01 的天数。
// 这样两个日期相减就是普通整数减法，不用自己逐月数天数。
static long daysFromCivil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return static_cast<long>(era) * 146097 + static_cast<long>(doe) - 719468;
}

Date Date::today() {
    time_t t = time(nullptr);
    tm now;
#if defined(_WIN32)
    localtime_s(&now, &t);
#else
    localtime_r(&t, &now);
#endif
    return Date(now.tm_year + 1900, now.tm_mon + 1, now.tm_mday);
}

long Date::days() const {
    return daysFromCivil(y, m, d);
}

string Date::str() const {
    char buf[16];
    snprintf(buf, sizeof buf, "%04d-%02d-%02d", y, m, d);
    return string(buf);
}

bool Date::parse(const string& s) {
    int yy, mm, dd;
    if (sscanf(s.c_str(), "%d-%d-%d", &yy, &mm, &dd) != 3) return false;
    if (yy < 1 || mm < 1 || mm > 12) return false;
    if (dd < 1 || dd > daysInMonth(yy, mm)) return false;   // 2月、大小月都管住
    y = yy;
    m = mm;
    d = dd;
    return true;
}
