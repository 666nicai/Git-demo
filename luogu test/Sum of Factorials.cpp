/*# P1009 [NOIP 1998 普及组] 阶乘之和

## 题目描述

用高精度计算出 $S = 1! + 2! + 3! + \cdots + n!$（$n \le 50$）。

其中 `!` 表示阶乘，定义为 $n!=n\times (n-1)\times (n-2)\times \cdots \times 1$。例如，$5! = 5 \times 4 \times 3 \times 2 \times 1=120$。

## 输入格式

一个正整数 $n$。

## 输出格式

一个正整数 $S$，表示计算结果。

## 输入输出样例 #1

### 输入 #1

```
3

```

### 输出 #1

```
9
```

## 说明/提示

**【数据范围】**

对于 $100 \%$ 的数据，$1 \le n \le 50$。

**【其他说明】**

注，《深入浅出基础篇》中使用本题作为例题，但是其数据范围只有 $n \le 20$，使用书中的代码无法通过本题。

如果希望通过本题，请继续学习第八章高精度的知识。

NOIP1998 普及组 第二题*/
// P1009 阶乘之和
#include <bits/stdc++.h>        // 万能头文件:一次引入所有标准库,竞赛常用
using namespace std;            // 使用标准命名空间,cin/cout/vector 就不用写 std:: 前缀

// 高精度乘法:大数 a(低位在前)整体乘以小整数 b
vector<int> mul(const vector<int>& a, int b) {   // a 是存大数的数组,b 是要乘的小整数
    vector<int> c;              // c:存放乘积结果的数组(同样低位在前)
    int carry = 0;              // carry:进位值,初始为 0(个位之前没有进位)
    for (int i = 0; i < (int)a.size(); i++) {    // 从个位(下标 0)往高位逐位遍历 a 的每一位
        carry += a[i] * b;      // 当前位:本位数 a[i] 乘 b,再加上低位传来的进位 carry
        c.push_back(carry % 10);// 取 carry 的个位作为这一位的结果数字,存入 c
        carry /= 10;            // 剩下的部分(carry/10)作为进位,传给更高一位
    }
    while (carry) {             // 所有位乘完后,如果最高位还有进位没处理完
        c.push_back(carry % 10);// 把进位的个位追加为新的一位数字
        carry /= 10;            // 继续除 10,看还有没有更高位的进位
    }
    return c;                   // 返回乘完的结果数组
}

// 高精度加法:两个大数 a、b(都低位在前)逐位相加
vector<int> add(const vector<int>& a, const vector<int>& b) {  // 参数:两个要相加的大数数组
    vector<int> c;              // c:存放加法结果的数组
    int carry = 0;              // carry:进位值,初始为 0
    for (int i = 0; i < (int)max(a.size(), b.size()); i++) {   // 循环到较长的那个数为止(短的自动补 0)
        int x = i < (int)a.size() ? a[i] : 0;   // 取 a 的这一位;若 a 已到头,就当作 0
        int y = i < (int)b.size() ? b[i] : 0;   // 取 b 的这一位;同理,不够就补 0
        carry += x + y;         // 本位的两个数字相加,再加上低位传来的进位
        c.push_back(carry % 10);// 结果的个位作为这一位数字存入 c
        carry /= 10;            // 剩下的作为进位传给更高一位
    }
    while (carry) {             // 最高位相加后如果还有进位
        c.push_back(carry % 10);// 追加一位新数字
        carry /= 10;            // 继续除 10,直到进位为 0
    }
    return c;                   // 返回加完的结果数组
}

int main() {                    // 主函数:程序入口
    int n;                      // n:题目输入,要求计算 1!+2!+...+n!
    cin >> n;                   // 读入 n
    vector<int> fact(1, 1), sum(1, 0);  // fact 初始为 1(即 1!),sum 初始为 0(都低位在前)
    for (int i = 1; i <= n; i++) {      // 从 i=1 到 n 循环,依次算每个阶乘并累加
        fact = mul(fact, i);    // 阶乘递推:fact = 上一个阶乘 × i,得到 i!
        sum = add(sum, fact);   // 累加:sum = sum + i!
    }
    for (int i = (int)sum.size() - 1; i >= 0; i--)  // 数组低位在前,所以必须倒着打印
        cout << sum[i];         // 从最高位到个位依次输出每一位数字
    cout << endl;               // 输出换行
    return 0;                   // 正常结束,返回 0
}