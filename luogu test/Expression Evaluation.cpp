/*# D 表达式求值

## 题目描述

给定一个中缀表达式，请编写程序计算该表达式的值。表达式包含+、-、*、/、^、(、)，所有运算均为二元运算，操作数均为正整数，但可能不止一位，不超过10位。运算结果为整数，值域为[-2^31, 2^31)。除法运算结果若为小数则进行截尾取整。若除法运算中除数为0，则输出INVALID。幂运算须自行实现，不允许调用pow等系统函数。

测试数据保证幂运算中指数为非负，底数不为0，且不会出现诸如n^a^b连续多次幂运算。

## 输入格式

输入为多行，每行为一个长度不超过1000的字符串，表示中缀表达式。

## 输出格式

对每个表达式输出一行：为一个整数（表达式的值）或一个字符串INVALID。

## 输入样例

```
5+(10*2)-6
8*(999+1)
1+5/(1-1)
7*2^3
```

## 输出样例

```
19
8000
INVALID
56
```*/
// D 表达式求值
#include<bits/stdc++.h>
using namespace std;

stack<long long> num;   // 数字栈
stack<char> op;         // 符号栈
bool err;               // 本行有没有除零

int pri(char c){        // 优先级,^ 最高
    if(c=='+'||c=='-') return 1;
    if(c=='*'||c=='/') return 2;
    return 3;
}

long long pw(long long x,long long k){  // 快速幂,题目不让用 pow
    long long r=1;
    while(k){
        if(k&1) r*=x;
        x*=x;k>>=1;
    }
    return r;
}

void calc(){
    long long y=num.top();num.pop();
    long long x=num.top();num.pop();
    char c=op.top();op.pop();
    if(c=='+') num.push(x+y);
    else if(c=='-') num.push(x-y);
    else if(c=='*') num.push(x*y);
    else if(c=='/'){
        if(y==0) err=1;     // 除数为 0 整行作废
        else num.push(x/y);
    }
    else num.push(pw(x,y));
}

int main(){
    string s;
    vector<string> ans;
    while(getline(cin,s)){
        if(s.find_first_not_of(" \t\r")==string::npos) continue;    // 空行
        while(!num.empty()) num.pop();
        while(!op.empty()) op.pop();
        err=0;
        int n=s.size();
        for(int i=0;i<n&&!err;i++){
            char c=s[i];
            if(isspace(c)) continue;
            if(isdigit(c)){     // 多位数一次读完
                long long x=0;
                while(i<n&&isdigit(s[i])) x=x*10+s[i++]-'0';
                i--;
                num.push(x);
            }
            else if(c=='(') op.push(c);
            else if(c==')'){
                while(op.top()!='('){
                    calc();
                    if(err) break;
                }
                if(!err) op.pop();
            }
            else{
                // 栈顶能先算就先算;^ 右结合,同级不弹
                while(!op.empty()&&op.top()!='('&&(pri(op.top())>pri(c)||(pri(op.top())==pri(c)&&c!='^'))){
                    calc();
                    if(err) break;
                }
                op.push(c);
            }
        }
        while(!err&&!op.empty()) calc();
        ans.push_back(err?"INVALID":to_string(num.top()));
    }
    for(int i=0;i<(int)ans.size();i++){
        if(i) cout<<"\n";
        cout<<ans[i];
    }
    return 0;
}
