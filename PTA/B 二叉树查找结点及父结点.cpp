/*# B 二叉树查找结点及父结点

## 题目描述

编写程序在二叉树中查找给定结点及父结点。二叉树结点的数据域值不等于 0 的整数。

## 输入格式

输入第 1 行为一组用空格间隔的整数，表示带空指针信息的二叉树先根序列，其中空指针用 0 表示。
例如 1 5 8 0 0 0 6 0 0 表示下图的二叉树。

        1
       / \
      5   6
     /
    8

第 2 行为整数 m，表示查询个数。接下来 m 行，每行为一个不等于 0 的整数 K，表示要查找的结点的数据值。
m 不超过 100，二叉树结点个数不超过 150000，高度不超过 6000。
输入数据保证二叉树各结点数据值互不相等。

## 输出格式

输出为 m 行，每行 1 个整数，表示被查找结点 K 的父结点数据值，
若二叉树中无结点 K 或结点 K 无父结点，则输出 0。

## 输入样例

1 5 8 0 0 0 6 0 0
3
8
1
6

## 输出样例

5
0
1
*/
#include<bits/stdc++.h>
using namespace std;

struct Node{
    int v;                      // 数据域
    Node *l,*r;                 // 左右孩子
    Node(int v):v(v),l(nullptr),r(nullptr){}
};

Node* build(){                  // 按带空指针的先序序列建树
    int x;
    if(!(cin>>x)) return nullptr;   // 输入结束
    if(x==0) return nullptr;        // 0 表示空指针,递归终点
    Node *p=new Node(x);
    p->l=build();                   // 紧接着的一段是左子树
    p->r=build();                   // 再紧接着的一段是右子树
    return p;
}

unordered_map<int,int> ans;     // 待查值 -> 父结点值,默认 0 即"无父/不存在"

void dfs(Node *p,int par){      // par 是父结点的值,根结点传 0
    if(!p) return;
    if(ans.count(p->v)) ans[p->v]=par;  // 只记录待查的那些值
    dfs(p->l,p->v);
    dfs(p->r,p->v);
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    Node *root=build();         // 序列自带边界,建完正好停在第一行末尾
    int m; cin>>m;
    vector<int> q(m);
    for(int i=0;i<m;i++){
        cin>>q[i];
        ans[q[i]]=0;            // 先当作没找到,遍历时找到了再覆盖
    }
    dfs(root,0);                // 一次遍历回答全部询问
    for(int i=0;i<m;i++)
        cout<<ans[q[i]]<<"\n";
    return 0;
}
