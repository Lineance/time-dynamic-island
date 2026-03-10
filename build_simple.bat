@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

echo ========================================
echo   灵动岛 Dynamic Island - 编译脚本
echo ========================================
echo.

REM 设置 Visual Studio 路径
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

REM 获取 MSBuild 路径
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set MSBUILD=%%i
)

echo 使用 MSBuild: %MSBUILD%
echo.

REM 创建输出目录
if not exist bin mkdir bin

REM 设置编译参数
set INCLUDES=/Iinclude /I"%WindowsSdkDir%Include\%WindowsSDKVersion%\um" /I"%WindowsSdkDir%Include\%WindowsSDKVersion%\shared"
set LIBS=/LIBPATH:"%WindowsSdkDir%Lib\%WindowsSDKVersion%\um\x64"

echo [1/3] 编译源文件...

REM 编译每个源文件
cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\main.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\WindowManager.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\Renderer.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\TimeService.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\ThemeMonitor.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\InteractionHandler.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\AntiCheatEvasion.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\StartupManager.cpp
if errorlevel 1 goto error

cl /c /std:c++17 /O2 /EHsc /W4 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN %INCLUDES% src\utils.cpp
if errorlevel 1 goto error

echo.
echo [2/3] 链接可执行文件...

REM 链接
link /nologo /OUT:bin\DynamicIsland.exe /SUBSYSTEM:WINDOWS main.obj WindowManager.obj Renderer.obj TimeService.obj ThemeMonitor.obj InteractionHandler.obj AntiCheatEvasion.obj StartupManager.obj utils.obj user32.lib gdi32.lib gdiplus.lib shell32.lib ole32.lib comctl32.lib dwmapi.lib
if errorlevel 1 goto error

echo.
echo [3/3] 清理临时文件...
del *.obj 2>nul

echo.
echo ========================================
echo   编译成功！
echo ========================================
echo.
echo 可执行文件位置：bin\DynamicIsland.exe
echo.

goto end

:error
echo.
echo ========================================
echo   编译失败！
echo ========================================
echo.
del *.obj 2>nul
exit /b 1

:end
endlocal
