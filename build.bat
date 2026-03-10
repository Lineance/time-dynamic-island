@echo off
chcp 65001 >nul
echo ========================================
echo   Dynamic Island - Build Library
echo ========================================
echo.

REM Find Visual Studio
set VSWHERE="%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
for /f "usebackq tokens=*" %%i in (`%VSWHERE% -latest -requires Microsoft.Component.MSBuild -property installationPath`) do set VSINSTALLDIR=%%i

REM Set up VC environment
call "%VSINSTALLDIR%\VC\Auxiliary\Build\vcvarsall.bat" amd64

echo.
echo Compiler: cl.exe
echo.

REM Create output directory for object files only
if not exist obj mkdir obj

REM Compile to object file only (no executable)
echo [1/1] Compiling main.cpp to object file...
cl /c /std:c++17 /O2 /EHsc /W3 /nologo /DUNICODE /D_UNICODE /DNOMINMAX /DWIN32_LEAN_AND_MEAN /Fo:obj\main.obj src\main.cpp
if errorlevel 1 goto error

echo.
echo ========================================
echo   Compilation SUCCESSFUL!
echo ========================================
echo.
echo Output: obj\main.obj (object file only)
echo No executable was created.
echo.
goto end

:error
echo.
echo ========================================
echo   Compilation FAILED!
echo ========================================
echo.
del obj\main.obj 2>nul
exit /b 1

:end
