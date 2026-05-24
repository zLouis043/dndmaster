@echo off
setlocal
cd /d "%~dp0\..\.."

echo === Pulizia Vecchia Build ===
if exist build rd /s /q build

echo === Bootstrapping vcpkg ===
call vendor\vcpkg\bootstrap-vcpkg.bat -disableMetrics
if %ERRORLEVEL% neq 0 (
    echo [ERRORE] Fallito il bootstrap di vcpkg.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo === Configuring CMake ===

cmake -G "Visual Studio 17 2022" -A x64 -B build -S . -DCMAKE_TOOLCHAIN_FILE=vendor/vcpkg/scripts/buildsystems/vcpkg.cmake
if %ERRORLEVEL% neq 0 (
    echo [ERRORE] Configurazione CMake fallita.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo === Setup Completato con Successo! ===
pause