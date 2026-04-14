@echo off
setlocal
cd /d "%~dp0\..\.."

echo === Creazione Pacchetto di Installazione (CPack) ===
cd build
cpack -C Release
if %ERRORLEVEL% neq 0 (
    echo [ERRORE] Creazione del pacchetto fallita.
    cd ..
    pause
    exit /b %ERRORLEVEL%
)
cd ..

echo.
echo === TUTTO PRONTO! ===
echo Trovi il file compresso/installer dentro la cartella 'build'.
pause