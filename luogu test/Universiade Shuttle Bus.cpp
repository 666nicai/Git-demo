/*# C 大冬会接驳车

## 题目描述

2027年世界大学生冬季运动会将在长春举行，组委会为运动员配备了无人驾驶的接驳车。假定你是组委会的软件工程师，请你为无人驾驶接驳车编写路径规划程序，使其载着运动员以最短的时间到达目的地。

接驳车的运行范围可以被建模为一张由 n 行 m 列单元格组成的地图。有的单元格是空地，可以走；有的单元格处是障碍物，不能走。假定接驳车只可以朝上、下、左、右四个方向行驶，不能斜着走。每行驶过一个位置（单元格）需要 1 分钟。给定地图以及运动员的起点和终点，请输出接驳车从起点行驶到终点所需的最短时间。

## 输入格式

输入包含多组数据。每组数据第一行是两个整数 n 和 m（1≤m,n≤100），表示地图的长和宽；接下来是 n 行，每行 m 个数字，表示整个地图。空地格子用 0 表示，障碍物用 1 表示，运动员所在起点用 3 表示，目的地用 4 表示。

## 输出格式

对于每组数据，如果接驳车能够到达目的地，输出一个整数，表示运动员从起点到目的地所需的最短时间。如果不能到达目的地，输出“unreachable”。

## 输入样例

```
5 5
1 0 1 1 1
1 0 4 1 0
1 0 0 1 0
0 0 0 1 0
1 0 3 0 1
5 5
3 0 1 1 1
1 0 1 1 0
1 0 1 1 0
0 0 0 1 0
1 0 1 0 4
```*/
// C 大冬会接驳车
#include<bits/stdc++.h>
using namespace std;

int g[105][105],d[105][105];            // g:地图  d:到起点的步数,-1 表示没走到过
int dx[4]={-1,1,0,0},dy[4]={0,0,-1,1};  // 四个方向:上、下、左、右

int main(){
    int n,m;
    vector<string> ans;         // 先存下每组答案,最后统一输出
    while(cin>>n>>m){           // 多组数据,读到文件尾
        int sx=0,sy=0,ex=0,ey=0;
        for(int i=0;i<n;i++)
            for(int j=0;j<m;j++){
                char c;
                cin>>c;         // 按字符读,数字之间有没有空格都不怕
                g[i][j]=c-'0';
                if(c=='3') sx=i,sy=j;
                if(c=='4') ex=i,ey=j;
            }
        memset(d,-1,sizeof(d)); // 每组都要重置,-1 表示没访问过
        queue<pair<int,int>> q;
        d[sx][sy]=0;
        q.push({sx,sy});
        while(!q.empty()){      // BFS 逐层扩,第一次走到就是最短
            int x=q.front().first,y=q.front().second;
            q.pop();
            for(int k=0;k<4;k++){
                int nx=x+dx[k],ny=y+dy[k];
                if(nx<0||nx>=n||ny<0||ny>=m) continue;   // 出界
                if(g[nx][ny]==1||d[nx][ny]!=-1) continue;   // 障碍或已经走过
                d[nx][ny]=d[x][y]+1;
                q.push({nx,ny});
            }
        }
        ans.push_back(d[ex][ey]<0?"unreachable":to_string(d[ex][ey]));
    }
    for(int i=0;i<(int)ans.size();i++){
        if(i) cout<<"\n";       // 换行放前面,最后一行不多换行
        cout<<ans[i];
    }
    return 0;
}
