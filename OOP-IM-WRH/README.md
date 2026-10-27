# 模拟即时通信系统（OOP 课程设计）

魏荣赫　软件一班（552501班）　学号 55250124

本项目是一个控制台版“腾讯立体社交平台”模拟程序，用 C++17 实现用户、微X账号、好友、群和群成员的管理，重点演示封装、继承、多态和文件持久化。

## 目录结构

```text
OOP-IM-WRH/
├─ README.md                 本文件
├─ 源代码/                   31 个源文件（14 个 .cpp + 17 个 .h）
├─ 文档/                     报告、实验报告、PPT、逐行解析、说明
├─ 构建/                     编译脚本、可执行文件、运行数据
│  ├─ build_windows.bat
│  ├─ sim_IMWRH.exe
│  └─ platform_data.txt      程序运行时自动读写的数据文件
├─ 工具/                     文档与 PPT 生成脚本
│  ├─ make_report.py
│  └─ make_ppt.py
└─ 素材/
   └─ report_assets/         报告用的三张 PNG 图
```

## 编译

### 方式一：一键脚本（推荐）

双击 `构建\build_windows.bat`，或在 cmd 中执行：

```bat
cd /d D:\Git-demo\OOP-IM-WRH\构建
.\build_windows.bat
```

脚本会从 `源代码\` 取源文件，先编译出临时的 `sim_IMWRH.build.exe`，成功后再替换 `构建\sim_IMWRH.exe`，所以编译失败不会留下损坏的 exe。

### 方式二：手动编译

在 `源代码` 目录打开 cmd：

```bat
cd /d D:\Git-demo\OOP-IM-WRH\源代码
g++ -std=c++17 -static -static-libgcc -static-libstdc++ -Wall ^
main.cpp PlatformWRH.cpp UserWRH.cpp MicroAccountWRH.cpp ^
QQAccountWRH.cpp WeiboAccountWRH.cpp WeChatAccountWRH.cpp ^
GroupWRH.cpp QQGroupWRH.cpp WeChatGroupWRH.cpp ^
FriendBookWRH.cpp GroupMemberBookWRH.cpp MyDateWRH.cpp BaseInfoWRH.cpp ^
-o ..\构建\sim_IMWRH.exe
```

若 `g++` 不在 PATH 中，可把它换成完整路径，例如 `C:\mingw64\bin\g++.exe`。

## 运行

```bat
cd /d D:\Git-demo\OOP-IM-WRH\构建
sim_IMWRH.exe
```

启动后输入 `1` 可观看“一条龙自动演示”，输入 `0` 退出。程序会在**当前工作目录**读写 `platform_data.txt`；因此建议先进入 `构建` 再运行，让数据文件落在 `构建\platform_data.txt`。删除该文件后，程序会重新使用内置演示数据。

## 文档索引

| 文件 | 内容 |
| --- | --- |
| `文档/课程设计报告_魏荣赫_55250124.docx` | 完整课程设计报告 |
| `文档/课程设计报告_魏荣赫_55250124.md` | 报告 Markdown 源稿 |
| `文档/实验报告_魏荣赫_55250124_封面版.docx` | 带封面的 5 页实验报告 |
| `文档/源码逐行解析_魏荣赫_55250124.docx` | 31 个源文件、3060 行代码的逐行讲解 |
| `文档/答辩PPT_魏荣赫_55250124.pptx` | 16 页答辩 PPT |
| `文档/编译运行指引.md` | 更详细的编译、运行和演示说明 |
| `文档/设计说明_类图.md` | 类关系、分层设计和持久化格式 |

## 重新生成文档

生成脚本依赖 `python-docx`、`python-pptx`、`matplotlib`：

```bat
cd /d D:\Git-demo\OOP-IM-WRH\工具
python make_report.py
python make_ppt.py
```

- `make_report.py` 会重画 `素材/report_assets/` 下的三张图，并覆盖 `文档/课程设计报告_魏荣赫_55250124.docx`；只画图可加 `--assets-only`。
- `make_ppt.py` 会覆盖 `文档/答辩PPT_魏荣赫_55250124.pptx`。
- `实验报告_魏荣赫_55250124_封面版.docx` 和 `源码逐行解析_魏荣赫_55250124.docx` 不由这两个脚本生成，修改时请直接编辑原文件。

## 源码入口

- `源代码/main.cpp`：设置控制台 UTF-8，构造 `PlatformWRH`，进入菜单。
- `源代码/PlatformWRH.cpp`：平台总控、菜单、自动演示和文件读写。
- `源代码/ShortTypes.h`：给 `.cpp` 用的短类型别名（`User` = `UserWRH` 等）。
