# 灵动岛编译脚本 - PowerShell 版本
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  灵动岛 Dynamic Island - 编译开始" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# 获取 MSBuild 路径
$vsWherePath = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$msbuildPath = & $vsWherePath -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe

Write-Host "MSBuild 路径：$msbuildPath" -ForegroundColor Gray
Write-Host ""

# 获取 Windows SDK 路径
$windowsSdkDir = ${env:WindowsSdkDir}
$windowsSDKVersion = ${env:WindowsSDKVersion}

Write-Host "Windows SDK: $windowsSDKVersion" -ForegroundColor Gray
Write-Host ""

# 创建输出目录
if (-not (Test-Path "bin")) {
    New-Item -ItemType Directory -Path "bin" | Out-Null
}

# 设置编译参数
$commonArgs = @(
    "/std:c++17", "/O2", "/EHsc", "/W4", "/nologo",
    "/DUNICODE", "/D_UNICODE", "/DNOMINMAX", "/DWIN32_LEAN_AND_MEAN",
    "/Iinclude",
    "/I${env:WindowsSdkDir}Include\${env:WindowsSDKVersion}\um",
    "/I${env:WindowsSdkDir}Include\${env:WindowsSDKVersion}\shared"
)

$sourceFiles = @(
    "src/main.cpp",
    "src/WindowManager.cpp",
    "src/Renderer.cpp",
    "src/TimeService.cpp",
    "src/ThemeMonitor.cpp",
    "src/InteractionHandler.cpp",
    "src/AntiCheatEvasion.cpp",
    "src/StartupManager.cpp",
    "src/utils.cpp"
)

Write-Host "[1/3] 编译源文件..." -ForegroundColor Cyan

foreach ($src in $sourceFiles) {
    $objFile = $src -replace "\.cpp$", ".obj"
    Write-Host "  Compiling: $src" -ForegroundColor Gray
    
    $clArgs = @("/c") + $commonArgs + @("/Fo:$objFile", $src)
    & cl.exe $clArgs 2>&1 | Out-String
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "错误：编译 $src 失败！" -ForegroundColor Red
        exit 1
    }
}

Write-Host ""
Write-Host "[2/3] 链接可执行文件..." -ForegroundColor Cyan

$linkArgs = @(
    "/nologo",
    "/OUT:bin\DynamicIsland.exe",
    "/SUBSYSTEM:WINDOWS",
    "main.obj", "WindowManager.obj", "Renderer.obj", "TimeService.obj", 
    "ThemeMonitor.obj", "InteractionHandler.obj", "AntiCheatEvasion.obj", 
    "StartupManager.obj", "utils.obj",
    "user32.lib", "gdi32.lib", "gdiplus.lib", "shell32.lib", 
    "ole32.lib", "comctl32.lib", "dwmapi.lib"
)

& link.exe $linkArgs

if ($LASTEXITCODE -ne 0) {
    Write-Host "错误：链接失败！" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "[3/3] 清理临时文件..." -ForegroundColor Cyan

Get-ChildItem -Path . -Filter "*.obj" | Remove-Item -Force

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  编译成功！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "可执行文件位置：bin\DynamicIsland.exe" -ForegroundColor Green
Write-Host ""
