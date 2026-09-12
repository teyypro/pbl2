@echo off
REM === Build & Run script cho PBL2 ===
REM Cach dung: build.bat
REM   -> Tim tat ca file .cpp trong src/, compile thanh main.exe va chay

echo [*] Dang compile du an...

REM Thu thap tat ca file .cpp trong thu muc src/ (ke ca thu muc con)
setlocal enabledelayedexpansion
set "CPP_FILES="
for /r "%~dp0src" %%f in (*.cpp) do (
    set "CPP_FILES=!CPP_FILES! "%%f""
)

REM Compile tat ca file .cpp thanh 1 file main.exe
g++ -o "%~dp0main.exe" %CPP_FILES% -I"%~dp0src" -I"C:\msys64\ucrt64\include\mariadb" -L"C:\msys64\ucrt64\lib" -lmariadb -lws2_32 -lshlwapi -std=c++17

if %errorlevel% neq 0 (
    echo [X] Compile THAT BAI!
    exit /b 1
)

echo [*] Compile thanh cong! Dang chay...
echo ========================================
"%~dp0main.exe"
echo ========================================
echo [*] Chuong trinh da ket thuc.
