/*# P1008 [NOIP 1998 普及组] 三连击

## 题目背景

本题为提交答案题，您可以写程序或手算在本机上算出答案后，直接提交答案文本，也可提交答案生成程序。

## 题目描述

将 $1, 2, \ldots , 9$ 共 $9$ 个数分成 $3$ 组，分别组成 $3$ 个三位数，且使这 $3$ 个三位数构成 $1 : 2 : 3$ 的比例，试求出所有满足条件的 $3$ 个三位数。

## 输入格式

无

## 输出格式

若干行，每行 $3$ 个数字。按照每行第 $1$ 个数字升序排列。

## 输入输出样例 #1

### 输入 #1

```
无
```

### 输出 #1

```
192 384 576
* * *
...

* * *
（剩余部分不予展示）
```

## 说明/提示

NOIP1998 普及组 第一题*/
//P1008
#include<stdio.h>
int check(int a,int b,int c){
    int digits[10]={0};
    int num,digit;
    num=a;
    while(num>0){
        digit=num%10;
        digits[digit]++;
        num/=10;
    }
    num=b;
    while(num>0){
        digit=num%10;
        digits[digit]++;
        num/=10;
    }
    num=c;
    while(num>0){
        digit=num%10;
        digits[digit]++;
        num/=10;
    }
    for(int i=1;i<=9;i++){
        if(digits[i]!=1){
            return 0;
        }
    }
    return 1;
}
int main(){
    int a,b,c;
    for(a=123;a<=329;a++){
        b=a*2;
        c=a*3;
        if(c>987) break;
        if(check(a,b,c)){
            printf("%d %d %d\n",a,b,c);
         }
     }
    return 0;
 }