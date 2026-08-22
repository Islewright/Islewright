@echo off
setlocal

set "PROJECT_ROOT=%~dp0"
set "SERVER_EXE=%PROJECT_ROOT%out\build\debug\server\Debug\islewright-server.exe"
set "MOCKCLIENT_EXE=%PROJECT_ROOT%out\build\debug\mockclient\Debug\islewright-mockclient.exe"

if not exist "%SERVER_EXE%" (
    echo [ERROR] Server executable not found.
    echo Build the debug preset first: cmake --build --preset debug
    exit /b 1
)

if not exist "%MOCKCLIENT_EXE%" (
    echo [ERROR] Mock client executable not found.
    echo Build the debug preset first: cmake --build --preset debug
    exit /b 1
)

echo [TEST] Starting server...
start "Islewright Server" cmd /c ""%SERVER_EXE%""

timeout /t 1 /nobreak >nul

echo [TEST] Starting mock client...
echo Run "world 42", then "origin" or "chunk x y". Type "help" for the command guide.
"%MOCKCLIENT_EXE%"

echo.
echo [TEST] Mock client finished.
echo Press Enter in the server window to stop the server.

endlocal
