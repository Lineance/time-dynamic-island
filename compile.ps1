# 灵动岛编译脚本
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  灵动岛 Dynamic Island - 编译脚本" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 设置构建类型
$BUILD_TYPE = if ($args[0]) { $args[0] } else { "Release" }
Write-Host "构建类型：$BUILD_TYPE" -ForegroundColor Green
Write-Host ""

# 查找 Visual Studio 安装路径
$vsWherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (-not (Test-Path $vsWherePath)) {
    Write-Host "错误：未找到 Visual Studio 安装！" -ForegroundColor Red
    Write-Host "请安装 Visual Studio 2022 并包含 C++ 桌面开发组件" -ForegroundColor Yellow
    exit 1
}

# 获取 MSBuild 路径
$msbuildPath = & $vsWherePath -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe
if (-not $msbuildPath) {
    Write-Host "错误：未找到 MSBuild！" -ForegroundColor Red
    exit 1
}

Write-Host "使用 MSBuild: $msbuildPath" -ForegroundColor Gray
Write-Host ""

# 创建构建目录
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# 配置 CMake (如果已安装)
$cmakeExe = Get-Command cmake -ErrorAction SilentlyContinue
if ($cmakeExe) {
    Write-Host "[1/3] 使用 CMake 配置..." -ForegroundColor Cyan
    Set-Location build
    & cmake .. -G "Visual Studio 17 2022" -A x64
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误：CMake 配置失败！" -ForegroundColor Red
        Set-Location ..
        exit 1
    }
    Set-Location ..
} else {
    Write-Host "警告：未找到 CMake，尝试直接编译..." -ForegroundColor Yellow
    Write-Host "提示：建议安装 CMake 以获得更好的构建体验" -ForegroundColor Gray
    exit 1
}

# 编译
Write-Host ""
Write-Host "[2/3] 开始编译..." -ForegroundColor Cyan
Set-Location build
& $msbuildPath DynamicIsland.sln /p:Configuration=$BUILD_TYPE /p:Platform=x64
if ($LASTEXITCODE -ne 0) {
    Write-Host "错误：编译失败！" -ForegroundColor Red
    Set-Location ..
    exit 1
}
Set-Location ..

# 复制可执行文件
Write-Host ""
Write-Host "[3/3] 复制可执行文件..." -ForegroundColor Cyan
$exePath = "build\bin\$BUILD_TYPE\DynamicIsland.exe"
if (Test-Path $exePath) {
    Copy-Item $exePath ".\DynamicIsland.exe" -Force
    Write-Host "成功！可执行文件已复制到：$pwd\DynamicIsland.exe" -ForegroundColor Green
} else {
    Write-Host "警告：未找到可执行文件 $exePath" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  编译完成！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
