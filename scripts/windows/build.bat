@echo off
setlocal
cd /d "%~dp0\..\.."

echo === Building DnDMaster ===
cmake --build build --config Debug
if %ERRORLEVEL% neq 0 (
    echo [ERRORE] Compilazione fallita.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo === Build completata con successo! ===
:: pause (opzionale, toglilo se lo esegui direttamente da VSCode/Terminale)