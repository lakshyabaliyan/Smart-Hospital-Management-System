@echo off
cd /d "%~dp0"

g++ user.cpp -o user.exe -lws2_32

if errorlevel 1 (
    echo Compilation failed.
    pause
    exit /b 1
)

start "AROGYA Server" user.exe

timeout /t 2 /nobreak >nul

start "" "http://127.0.0.1:8080"

pause