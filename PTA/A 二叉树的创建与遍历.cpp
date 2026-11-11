/*# A 二叉树的创建与遍历

## 题目描述

通过带空指针信息的先根序列（即先序序列）创建二叉树，并进行先根（先序）、中根（中序）、后根（后序）遍历。二叉树结点数据域为不等于 0 的整数（可能是正数也可能是负数），空指针用 0 表示。例如 1 5 8 0 0 0 6 0 0 表示下图的二叉树。

```
        1
       / \
      5   6
     /
    8
```

## 输入格式

输入为一组用空格间隔的整数，表示带空指针信息的二叉树先根序列。其中空指针信息用 0 表示。二叉树结点个数不超过 15000。输入数据保证二叉树各结点数据值互不相等。

## 输出格式

输出为 3 行整数，每个整数后一个空格。第 1 行为该二叉树的先根序列，第 2 行为中根序列，第 3 行为后根序列。

## 输入样例

```
1 5 8 0 0 0 6 0 0
```

## 输出样例

```
1 5 8 6
8 5 1 6
8 5 6 1
```
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

void pre(Node *p){              // 先根:根 左 右
    if(!p) return;
    cout<<p->v<<" ";
    pre(p->l);
    pre(p->r);
}

void in(Node *p){               // 中根:左 根 右
    if(!p) return;
    in(p->l);
    cout<<p->v<<" ";
    in(p->r);
}

void post(Node *p){             // 后根:左 右 根
    if(!p) return;
    post(p->l);
    post(p->r);
    cout<<p->v<<" ";
}

int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    Node *root=build();
    pre(root);  cout<<"\n";     // 第 1 行:先根序列
    in(root);   cout<<"\n";     // 第 2 行:中根序列
    post(root); cout<<"\n";     // 第 3 行:后根序列
    return 0;
}
