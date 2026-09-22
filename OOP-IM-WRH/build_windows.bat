@echo off
rem ============================================================
rem  一键编译脚本（Windows 下的 g++）
rem  依次尝试：当前PATH中的g++ -> C:\mingw64 -> D:\msys64\ucrt64
rem ============================================================
set SRC=main.cpp PlatformWRH.cpp UserWRH.cpp MicroAccountWRH.cpp ^
QQAccountWRH.cpp WeiboAccountWRH.cpp WeChatAccountWRH.cpp ^
GroupWRH.cpp QQGroupWRH.cpp WeChatGroupWRH.cpp ^
FriendBookWRH.cpp GroupMemberBookWRH.cpp MyDateWRH.cpp BaseInfoWRH.cpp
set OUT=sim_IMWRH.exe

rem -static: 编译为免依赖运行库的静态版本，双击 .exe 即可直接运行
where g++ >nul 2>nul
if %errorlevel%==0 (
    echo [1] 使用 PATH 中的 g++
    g++ -std=c++17 -static -static-libgcc -static-libstdc++ -Wall %SRC% -o %OUT%
    if not errorlevel 1 goto ok
)

if exist "C:\mingw64\bin\g++.exe" (
    echo [2] 使用 C:\mingw64\bin\g++.exe
    "C:\mingw64\bin\g++.exe" -std=c++17 -static -static-libgcc -static-libstdc++ -Wall %SRC% -o %OUT%
    if not errorlevel 1 goto ok
)

if exist "D:\msys64\ucrt64\bin\g++.exe" (
    echo [3] 使用 D:\msys64\ucrt64\bin\g++.exe
    "D:\msys64\ucrt64\bin\g++.exe" -std=c++17 -static -static-libgcc -static-libstdc++ -Wall %SRC% -o %OUT%
    if not errorlevel 1 goto ok
)

echo 编译失败：未找到可用的 g++。
exit /b 1

:ok
echo 编译成功：%OUT%
echo 运行前若中文乱码，可在命令行先执行  chcp 65001
