@echo off
setlocal
pushd "%~dp0"

echo Building project...
g++ -std=c++17 -o app.exe main.cpp utils/*.cpp models/*.cpp data/*.cpp business/*.cpp presentation/*.cpp
if errorlevel 1 (
    echo Build failed. Check the errors above and ensure g++ is installed and available in PATH.
    echo Close any running app.exe before rebuilding.
    pause
    popd
    exit /b 1
)

app.exe
set "APP_EXIT=%errorlevel%"
popd
exit /b %APP_EXIT%
