# Compile Debug Version
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Compile Debug Version" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Setup VC environment
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vsWhere -latest -requires Microsoft.Component.MSBuild -property installationPath
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

Write-Host "Compiling src\main_debug.cpp..." -ForegroundColor Gray

# Compile
$compilerArgs = @(
    "/c"
    "/O2"
    "/EHsc"
    "/W3"
    "/nologo"
    "/DUNICODE"
    "/D_UNICODE"
    "/DNOMINMAX"
    "/DWIN32_LEAN_AND_MEAN"
    "/Zc:__cplusplus"
    "/std:c++17"
    "/Fo:obj\main_debug.obj"
    "/I$sdkInclude"
    "/I$sdkShared"
    "src\main_debug.cpp"
)

& cl.exe @compilerArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "Compilation successful! Linking..." -ForegroundColor Green
    
    # Link
    $linkArgs = @(
        "/nologo"
        "/OUT:DynamicIsland_debug.exe"
        "/SUBSYSTEM:WINDOWS"
        "/ENTRY:wWinMainCRTStartup"
        "obj\main_debug.obj"
        "user32.lib", "gdi32.lib", "shell32.lib", "ole32.lib",
        "comctl32.lib", "dwmapi.lib", "advapi32.lib"
    )
    
    & link.exe @linkArgs
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host ""
        Write-Host "========================================" -ForegroundColor Green
        Write-Host "  SUCCESS! Created: DynamicIsland_debug.exe" -ForegroundColor Green
        Write-Host "========================================" -ForegroundColor Green
        Write-Host ""
        
        if (Test-Path "DynamicIsland_debug.exe") {
            $fileSize = (Get-Item "DynamicIsland_debug.exe").Length
            Write-Host "Size: $([math]::Round($fileSize / 1KB, 2)) KB" -ForegroundColor Green
            Write-Host "Location: $pwd\DynamicIsland_debug.exe" -ForegroundColor Cyan
        }
        Write-Host ""
        Write-Host "Run with: .\DynamicIsland_debug.exe" -ForegroundColor Yellow
        Write-Host ""
    } else {
        Write-Host "Linking failed!" -ForegroundColor Red
        exit 1
    }
} else {
    Write-Host "Compilation failed!" -ForegroundColor Red
    exit 1
}
