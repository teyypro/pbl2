@echo off
setlocal
chcp 65001 >nul
pushd "%~dp0"
if errorlevel 1 exit /b 1

rem Use MSYS2 when g++ is not already on PATH.
where g++ >nul 2>&1
if errorlevel 1 if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
where g++ >nul 2>&1
if errorlevel 1 (
    echo Khong tim thay g++. Hay cai MinGW-w64/MSYS2 va them vao PATH.
    goto :failed
)

if not exist "build" mkdir "build"
if not exist "build" goto :failed

echo Dang bien dich...
g++ -std=c++14 -Wall -Wextra -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp business\*.cpp models\*.cpp orm\*.cpp presentation\*.cpp utils\*.cpp -o "build\StudentDocumentManagement.exe"
if errorlevel 1 goto :failed

echo Bien dich thanh cong: build\StudentDocumentManagement.exe
if /i "%~1"=="--build-only" (
    popd
    exit /b 0
)

echo.
"build\StudentDocumentManagement.exe"
set "APP_EXIT_CODE=%ERRORLEVEL%"
echo.
pause
popd
exit /b %APP_EXIT_CODE%

:failed
echo Bien dich that bai.
if /i not "%~1"=="--build-only" pause
popd
exit /b 1
