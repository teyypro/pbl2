@echo off
setlocal enabledelayedexpansion
chcp 65001 >nul

REM ============================================================
REM build.bat - Build & run Student Document System (Windows)
REM Yêu cầu: g++ trong PATH (MinGW / MSYS2 / TDM-GCC)
REM Cách dùng:
REM     build.bat           -> build ra app.exe
REM     build.bat run       -> build + chạy
REM     build.bat clean     -> xóa file build
REM     build.bat rebuild   -> clean + build
REM ============================================================

set "APP=app.exe"
set "CXX=g++"
set "CXXFLAGS=-std=c++11 -Wall -Wextra -O2"
set "SRC=main.cpp utils\*.cpp orm\*.cpp models\*.cpp business\*.cpp presentation\*.cpp"

REM ---------- Kiểm tra g++ ----------
where %CXX% >nul 2>nul
if errorlevel 1 (
    echo [LOI] Khong tim thay g++ trong PATH.
    echo       Cai MinGW-w64 hoac MSYS2, hoac them thu muc bin cua g++ vao PATH.
    echo       Vi du: C:\msys64\ucrt64\bin
    exit /b 1
)

REM ---------- Xử lý tham số ----------
if /i "%~1"=="clean"   goto :clean
if /i "%~1"=="rebuild" goto :rebuild
if /i "%~1"=="run"     goto :build_run

goto :build

REM ============================================================
:clean
echo [1/1] Xoa file build...
if exist "%APP%" del /q "%APP%"
if exist "*.o" del /q "*.o"
echo Da xoa.
exit /b 0

REM ============================================================
:rebuild
call "%~f0" clean
if errorlevel 1 exit /b 1
goto :build

REM ============================================================
:build_run
call :do_build
if errorlevel 1 exit /b 1
echo.
echo ============================================================
echo  Chay chuong trinh...
echo ============================================================
"%APP%"
exit /b %errorlevel%

REM ============================================================
:build
call :do_build
exit /b %errorlevel%

REM ============================================================
:do_build
echo ============================================================
echo  Build Student Document System
echo  Compiler : %CXX%
echo  Flags    : %CXXFLAGS%
echo  Output   : %APP%
echo ============================================================
echo.

if not exist "data" (
    echo [CANH BAO] Khong tim thay thu muc data\. Tao thu muc rong...
    mkdir data
)

REM --- Buoc dong app.exe cu neu dang chay ---
taskkill /f /im "%APP%" >nul 2>&1

echo [1/2] Bien dich...
%CXX% %CXXFLAGS% -o "%APP%" %SRC%
if errorlevel 1 (
    echo.
    echo [LOI] Bien dich that bai.
    exit /b 1
)

echo [2/2] Build thanh cong: %APP%
echo.
echo Chay thu: build.bat run
exit /b 0