# Link DynamicIsland.exe
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Linking DynamicIsland.exe" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Setup VC environment
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsPath = & $vsWhere -latest -requires Microsoft.Component.MSBuild -property installationPath
$vcVarsAll = "$vsPath\VC\Auxiliary\Build\vcvarsall.bat"

# Get environment variables
$envOutput = cmd /c "call `"$vcVarsAll`" amd64 && set"
foreach ($line in $envOutput) {
    if ($line -match '^(\w+)=(.*)$') {
        [Environment]::SetEnvironmentVariable($matches[1], $matches[2])
    }
}

Write-Host "Linking obj\main.obj..." -ForegroundColor Gray
Write-Host ""

# Link with correct entry point
$linkArgs = @(
    "/nologo",
    "/OUT:DynamicIsland.exe",
    "/SUBSYSTEM:WINDOWS",
    "/ENTRY:wWinMainCRTStartup",  # Correct entry point for Unicode WinMain
    "obj\main.obj",
    "user32.lib", "gdi32.lib", "shell32.lib", "ole32.lib",
    "comctl32.lib", "dwmapi.lib", "advapi32.lib"
)

& link.exe $linkArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "  SUCCESS!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Created: DynamicIsland.exe" -ForegroundColor Green
    
    if (Test-Path "DynamicIsland.exe") {
        $fileSize = (Get-Item "DynamicIsland.exe").Length
        Write-Host "Size: $([math]::Round($fileSize / 1KB, 2)) KB" -ForegroundColor Green
        Write-Host "Location: $pwd\DynamicIsland.exe" -ForegroundColor Cyan
    }
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "  Linking FAILED" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host ""
    exit 1
}
