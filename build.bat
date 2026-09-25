@echo off
REM === Build & Run script cho PBL2 ===
REM Cach dung: build.bat

cd /d "%~dp0"

echo [*] Dang compile du an...

g++ -o main.exe src/*.cpp -Isrc -std=c++17

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
