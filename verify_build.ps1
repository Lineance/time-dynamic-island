# Build Verification Script
# This script verifies that no .exe files are created during compilation

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Build Verification" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check for .exe files in the project directory
Write-Host "Checking for .exe files..." -ForegroundColor Yellow
$exeFiles = Get-ChildItem -Path . -Filter "*.exe" -Recurse -ErrorAction SilentlyContinue

if ($exeFiles.Count -gt 0) {
    Write-Host ""
    Write-Host "ERROR: Found .exe files in project directory!" -ForegroundColor Red
    Write-Host ""
    Write-Host "The following .exe files were found:" -ForegroundColor Yellow
    $exeFiles | ForEach-Object {
        Write-Host "  $($_.FullName)" -ForegroundColor Red
    }
    Write-Host ""
    Write-Host "Please remove these files and rebuild." -ForegroundColor Yellow
    exit 1
} else {
    Write-Host "SUCCESS: No .exe files found in project directory" -ForegroundColor Green
    Write-Host ""
}

# Check for object files (these are expected)
Write-Host "Checking for object files (*.obj)..." -ForegroundColor Yellow
$objFiles = Get-ChildItem -Path . -Filter "*.obj" -Recurse -ErrorAction SilentlyContinue

if ($objFiles.Count -gt 0) {
    Write-Host "Found $($objFiles.Count) object file(s):" -ForegroundColor Green
    $objFiles | ForEach-Object {
        Write-Host "  $($_.FullName)" -ForegroundColor Gray
    }
    Write-Host ""
} else {
    Write-Host "No object files found (this is OK if not compiled yet)" -ForegroundColor Yellow
    Write-Host ""
}

# Check for library files (these are OK)
Write-Host "Checking for library files (*.lib)..." -ForegroundColor Yellow
$libFiles = Get-ChildItem -Path . -Filter "*.lib" -Recurse -ErrorAction SilentlyContinue

if ($libFiles.Count -gt 0) {
    Write-Host "Found $($libFiles.Count) library file(s):" -ForegroundColor Green
    $libFiles | ForEach-Object {
        Write-Host "  $($_.FullName)" -ForegroundColor Gray
    }
    Write-Host ""
} else {
    Write-Host "No library files found" -ForegroundColor Yellow
    Write-Host ""
}

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Verification Complete" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Summary:" -ForegroundColor Cyan
Write-Host "  - Executable files (.exe): $($exeFiles.Count) (Expected: 0)" -ForegroundColor $(if ($exeFiles.Count -eq 0) { "Green" } else { "Red" })
Write-Host "  - Object files (.obj): $($objFiles.Count) (Expected: >= 0)" -ForegroundColor "Green"
Write-Host "  - Library files (.lib): $($libFiles.Count) (Expected: >= 0)" -ForegroundColor "Green"
Write-Host ""

if ($exeFiles.Count -eq 0) {
    Write-Host "PASSED: Build configuration is correct - no executables created" -ForegroundColor Green
    exit 0
} else {
    Write-Host "FAILED: Executables were created - check build configuration" -ForegroundColor Red
    exit 1
}
