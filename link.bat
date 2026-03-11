@echo off
chcp 65001 >nul
echo ========================================
echo   链接 DynamicIsland.exe
echo ========================================
echo.

REM 设置 Visual Studio 环境
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -requires Microsoft.Component.MSBuild -property installationPath`) do set VSINSTALLDIR=%%i
call "%VSINSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo.
echo 链接对象文件...
link /nologo /OUT:DynamicIsland.exe /SUBSYSTEM:WINDOWS ^
    obj\main.obj ^
    user32.lib gdi32.lib shell32.lib ole32.lib ^
    comctl32.lib dwmapi.lib advapi32.lib

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo   成功！已创建 DynamicIsland.exe
    echo ========================================
    echo.
    dir DynamicIsland.exe
) else (
    echo.
    echo ========================================
    echo   链接失败！
    echo ========================================
)
