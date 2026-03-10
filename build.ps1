# Dynamic Island - Compile to Object File Only
# No executable will be generated

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Dynamic Island - Compile Library" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Find Visual Studio installation
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vsWhere -latest -requires Microsoft.Component.MSBuild -property installationPath

Write-Host "Visual Studio Path: $vsPath" -ForegroundColor Gray

# Set up VC environment
$vcVarsAll = "$vsPath\VC\Auxiliary\Build\vcvarsall.bat"

# Get environment variables from vcvarsall.bat
$envOutput = cmd /c "call `"$vcVarsAll`" amd64 && set"
foreach ($line in $envOutput) {
    if ($line -match '^(\w+)=(.*)$') {
        [Environment]::SetEnvironmentVariable($matches[1], $matches[2])
    }
}

Write-Host ""
Write-Host "Compiler: cl.exe" -ForegroundColor Gray
Write-Host ""

# Get Windows SDK paths
$sdkIncludePath = "${env:WindowsSdkDir}Include\${env:WindowsSDKVersion}\um"
$sdkSharedPath = "${env:WindowsSdkDir}Include\${env:WindowsSDKVersion}\shared"

# Create output directory for object files
if (-not (Test-Path "obj")) {
    New-Item -ItemType Directory -Path "obj" | Out-Null
}

Write-Host "[1/1] Compiling main.cpp to object file..." -ForegroundColor Cyan

$compileFlags = "/c /std:c++17 /O2 /EHsc /W3 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN /Zc:__cplusplus"

& cl.exe $compileFlags /Fo:"obj\main.obj" /I"include" /I"$sdkIncludePath" /I"$sdkSharedPath" src\main.cpp

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Host "Error: Compilation failed!" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Compilation SUCCESSFUL!" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Output: obj\main.obj (object file only)" -ForegroundColor Green
Write-Host "No executable was created." -ForegroundColor Yellow
Write-Host ""

# Verify no .exe files were created
$exeFiles = Get-ChildItem -Path . -Filter "*.exe" -Recurse -ErrorAction SilentlyContinue
if ($exeFiles.Count -gt 0) {
    Write-Host "Warning: Found .exe files:" -ForegroundColor Yellow
    $exeFiles | ForEach-Object { Write-Host "  $($_.FullName)" }
    Write-Host ""
} else {
    Write-Host "Verified: No .exe files in project directory" -ForegroundColor Green
    Write-Host ""
}
