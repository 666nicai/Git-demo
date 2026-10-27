@echo off
rem Switch console to UTF-8 so Chinese messages do not garble
chcp 65001 >nul

rem ============================================================
rem  One-click build script (g++ on Windows)
rem  Prefer g++ in PATH; fall back to C:\mingw64 / D:\msys64.
rem  Compile to a temp file first, then replace sim_IMWRH.exe,
rem  so a failed build never leaves a 0-byte exe behind.
rem ============================================================

rem Build output stays next to this script; sources are in ../源代码
cd /d "%~dp0"

set SRC_DIR=%~dp0..\源代码
set SRC="%SRC_DIR%\main.cpp" "%SRC_DIR%\PlatformWRH.cpp" "%SRC_DIR%\UserWRH.cpp" "%SRC_DIR%\MicroAccountWRH.cpp" ^
    "%SRC_DIR%\QQAccountWRH.cpp" "%SRC_DIR%\WeiboAccountWRH.cpp" "%SRC_DIR%\WeChatAccountWRH.cpp" ^
    "%SRC_DIR%\GroupWRH.cpp" "%SRC_DIR%\QQGroupWRH.cpp" "%SRC_DIR%\WeChatGroupWRH.cpp" ^
    "%SRC_DIR%\FriendBookWRH.cpp" "%SRC_DIR%\GroupMemberBookWRH.cpp" "%SRC_DIR%\MyDateWRH.cpp" ^
    "%SRC_DIR%\BaseInfoWRH.cpp"
set OUT=sim_IMWRH.exe
set TMPOUT=sim_IMWRH.build.exe

rem Find one usable compiler only; do not retry the others
set GXX=
where g++ >nul 2>nul
if not errorlevel 1 set GXX=g++
if not defined GXX if exist "C:\mingw64\bin\g++.exe" set GXX=C:\mingw64\bin\g++.exe
if not defined GXX if exist "D:\msys64\ucrt64\bin\g++.exe" set GXX=D:\msys64\ucrt64\bin\g++.exe
if not defined GXX goto nogxx

echo 使用编译器：%GXX%
echo 正在编译，请稍候（大约 10~30 秒）...
echo.

rem -static: build a self-contained exe that runs without extra DLLs
"%GXX%" -std=c++17 -static -static-libgcc -static-libstdc++ -Wall %SRC% -o "%TMPOUT%"
if errorlevel 1 goto buildfail

rem Replace the official exe only after a successful build
move /Y "%TMPOUT%" "%OUT%" >nul
if errorlevel 1 goto movefail

echo.
echo 编译成功：%OUT%
for %%A in ("%OUT%") do echo 文件大小：%%~zA 字节
echo 运行前若中文乱码，可在命令行先执行  chcp 65001
echo.
pause
exit /b 0

:buildfail
echo.
echo 编译失败：g++ 返回了错误，请看上面的报错信息。
if exist "%TMPOUT%" del "%TMPOUT%"
echo 原来的 %OUT% 没有被改动，可以继续用旧版本。
pause
exit /b 1

:movefail
echo.
echo 编译成功，但替换 %OUT% 失败（文件可能正被占用）。
echo 新文件保留在 %TMPOUT%，请关闭正在运行的 sim_IMWRH.exe 后重试。
pause
exit /b 1

:nogxx
echo 编译失败：未找到可用的 g++。
echo 请安装 MinGW-w64，或把 g++.exe 放到 C:\mingw64\bin\ 下。
pause
exit /b 1
