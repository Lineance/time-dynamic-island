# Dynamic Island - Simple Compile Script
# Compiles to object file only (no executable)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Dynamic Island - Compile" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Find Visual Studio
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vsWhere -latest -requires Microsoft.Component.MSBuild -property installationPath

Write-Host "Visual Studio: $vsPath" -ForegroundColor Gray

# Setup VC environment
$vcVarsAll = "$vsPath\VC\Auxiliary\Build\vcvarsall.bat"
$envOutput = cmd /c "call `"$vcVarsAll`" amd64 && set"
foreach ($line in $envOutput) {
    if ($line -match '^(\w+)=(.*)$') {
        [Environment]::SetEnvironmentVariable($matches[1], $matches[2])
    }
}

# Get SDK paths
$sdkInclude = "${env:WindowsSdkDir}Include\${env:WindowsSDKVersion}\um"
$sdkShared = "${env:WindowsSdkDir}Include\${env:WindowsSDKVersion}\shared"

# Create output directory
if (-not (Test-Path "obj")) {
    New-Item -ItemType Directory -Path "obj" | Out-Null
}

Write-Host ""
Write-Host "Compiling src\main.cpp..." -ForegroundColor Cyan

# Compile with simpler flags
$flags = "/c /O2 /EHsc /W3 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN /Zc:__cplusplus /std:c++17"

& cl.exe $flags /Fo:"obj\main.obj" /I"$sdkInclude" /I"$sdkShared" src\main.cpp

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  Compilation SUCCESSFUL!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Output: obj\main.obj" -ForegroundColor Green
    Write-Host "No executable created (compile only)" -ForegroundColor Yellow
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "  Compilation FAILED" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host ""
    exit 1
}
