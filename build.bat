@echo off
REM === Build & Run script cho PBL2 ===
REM Dung file .txt de luu tru du lieu (khong can MySQL)
REM Cach dung: build.bat

cd /d "%~dp0"

echo [*] Dang compile du an...

g++ -o main.exe src/main.cpp -Isrc -Isrc/database -Isrc/models -Isrc/repositories -Isrc/services -Isrc/controllers -Isrc/utils -std=c++17

if %errorlevel% neq 0 (
    echo [X] Compile THAT BAI!
    pause
    exit /b 1
)

echo [*] Compile thanh cong! Dang khoi chay chuong trinh...
echo ======================================================
main.exe
echo ======================================================
echo [*] Chuong trinh da ket thuc.
pause
