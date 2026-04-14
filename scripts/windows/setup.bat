@echo off
setlocal
cd /d "%~dp0\..\.."

echo === Bootstrapping vcpkg ===
call vcpkg\bootstrap-vcpkg.bat -disableMetrics
if %ERRORLEVEL% neq 0 (
    echo [ERRORE] Fallito il bootstrap di vcpkg.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo === Configuring CMake ===
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% neq 0 (
    echo [ERRORE] Configurazione CMake fallita.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo === Setup Completato con Successo! ===
pause