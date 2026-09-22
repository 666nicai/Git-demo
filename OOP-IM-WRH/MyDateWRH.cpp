// 作者：魏荣赫 学号55250124 软件一班(552401班)  类名统一带姓名缩写 WRH
// 文件：MyDateWRH.cpp
#include "MyDateWRH.h"
#include <cstdio>
#include <ctime>

// 计算自 epoch 起的天数（Howard Hinnant civil 算法）
static long daysFromCivil(int y, unsigned m, unsigned d) {
    y -= m <= 2;
    const int era = (y >= 0 ? y : y - 399) / 400;
    const unsigned yoe = static_cast<unsigned>(y - era * 400);
    const unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return static_cast<long>(era) * 146097 + static_cast<long>(doe) - 719468;
}

MyDateWRH MyDateWRH::today() {
    std::time_t t = std::time(nullptr);
    std::tm tm;
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return MyDateWRH(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
}

long MyDateWRH::days() const {
    return daysFromCivil(y, m, d);
}

std::string MyDateWRH::str() const {
    char buf[16];
    std::snprintf(buf, sizeof buf, "%04d-%02d-%02d", y, m, d);
    return std::string(buf);
}

bool MyDateWRH::parse(const std::string& s) {
    int yy, mm, dd;
    if (std::sscanf(s.c_str(), "%d-%d-%d", &yy, &mm, &dd) != 3) return false;
    if (mm < 1 || mm > 12 || dd < 1 || dd > 31) return false;
    y = yy; m = mm; d = dd;
    return true;
}
