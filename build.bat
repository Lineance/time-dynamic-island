@echo off
chcp 65001 >nul
echo ========================================
echo   灵动岛 Dynamic Island - 构建脚本
echo ========================================
echo.

REM 检查是否指定了构建类型
if "%1"=="" (
    set BUILD_TYPE=Release
) else (
    set BUILD_TYPE=%1
)

echo 构建类型：%BUILD_TYPE%
echo.

REM 创建构建目录
if not exist build mkdir build
cd build

REM 配置 CMake
echo [1/3] 配置 CMake...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo.
    echo 错误：CMake 配置失败！
    cd ..
    exit /b 1
)

echo.
echo [2/3] 编译项目...
cmake --build . --config %BUILD_TYPE%
if errorlevel 1 (
    echo.
    echo 错误：编译失败！
    cd ..
    exit /b 1
)

echo.
echo [3/3] 复制可执行文件...
if exist bin\%BUILD_TYPE%\DynamicIsland.exe (
    copy bin\%BUILD_TYPE%\DynamicIsland.exe ..\DynamicIsland.exe >nul
    echo 成功！可执行文件已复制到项目根目录
    echo 文件位置：%~dp0DynamicIsland.exe
) else (
    echo 警告：未找到可执行文件
)

cd ..
echo.
echo ========================================
echo   构建完成！
echo ========================================
