@echo off
setlocal

cd /d "%~dp0"

echo [TEST] Building Islewright debug targets...
cmake --build --preset debug
if errorlevel 1 (
    echo [FAIL] Build failed.
    exit /b 1
)

echo.
echo [TEST] Running all automated tests...
ctest --test-dir out\build\debug -C Debug --output-on-failure
if errorlevel 1 (
    echo [FAIL] One or more tests failed.
    exit /b 1
)

echo.
echo [PASS] Build and all automated tests succeeded.
echo Run test_chunk.cmd for the interactive server and mock client test.

endlocal
