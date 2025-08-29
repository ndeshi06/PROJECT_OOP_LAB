@echo off
setlocal EnableDelayedExpansion

:: Set paths
set SRC_DIR=src
set BUILD_DIR=build
set OBJ_DIR=build\obj
set INCLUDE_DIR=src\include

echo ========================================
echo Building Console Version
echo ========================================

:: Check if g++ is available
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW-w64 or add g++ to PATH
    pause
    exit /b 1
)

:: Compiler flags for console version
set CXX_FLAGS=-std=c++17 -O2
set INCLUDE_FLAGS=-I%INCLUDE_DIR%

:: Clean build directory if it exists
if exist build (
    echo Cleaning existing build directory...
    rmdir /s /q build
)

if exist data (
    echo Cleaning existing data directory...
    rmdir /s /q data
)

:: Create build directory
if not exist build mkdir build
if not exist build\obj mkdir build\obj

echo.
echo Compiler: 
g++ --version

echo.
echo ========================================
echo Compiling source files...
echo ========================================

:: Compile models
echo Compiling models...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\models\User.cpp -o %OBJ_DIR%\User.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\models\Court.cpp -o %OBJ_DIR%\Court.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\models\Booking.cpp -o %OBJ_DIR%\Booking.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\models\Statistics.cpp -o %OBJ_DIR%\Statistics.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile controllers
echo Compiling controllers...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\controllers\AuthController.cpp -o %OBJ_DIR%\AuthController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\controllers\CourtController.cpp -o %OBJ_DIR%\CourtController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\controllers\BookingController.cpp -o %OBJ_DIR%\BookingController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\controllers\StatisticsController.cpp -o %OBJ_DIR%\StatisticsController.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile patterns
echo Compiling design patterns...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\patterns\BookingManager.cpp -o %OBJ_DIR%\BookingManager.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\patterns\NotificationObserver.cpp -o %OBJ_DIR%\NotificationObserver.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile utilities
echo Compiling utilities...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\utils\Database.cpp -o %OBJ_DIR%\Database.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\utils\DateTimeUtils.cpp -o %OBJ_DIR%\DateTimeUtils.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile console main
echo Compiling console main application...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\console_main.cpp -o %OBJ_DIR%\console_main.o
if %ERRORLEVEL% neq 0 goto :error

echo.
echo ========================================
echo Linking console executable...
echo ========================================

:: Link all object files for console version
g++ %CXX_FLAGS% ^
    %OBJ_DIR%\User.o ^
    %OBJ_DIR%\Court.o ^
    %OBJ_DIR%\Booking.o ^
    %OBJ_DIR%\Statistics.o ^
    %OBJ_DIR%\AuthController.o ^
    %OBJ_DIR%\CourtController.o ^
    %OBJ_DIR%\BookingController.o ^
    %OBJ_DIR%\StatisticsController.o ^
    %OBJ_DIR%\BookingManager.o ^
    %OBJ_DIR%\NotificationObserver.o ^
    %OBJ_DIR%\Database.o ^
    %OBJ_DIR%\DateTimeUtils.o ^
    %OBJ_DIR%\console_main.o ^
    -o %BUILD_DIR%\BadmintonCourtManagerConsole.exe

if %ERRORLEVEL% neq 0 goto :error

echo.
echo ========================================
echo Console Build completed successfully!
echo ========================================
echo Executable: %BUILD_DIR%\BadmintonCourtManagerConsole.exe
echo.
echo To run the console application:
echo   cd build
echo   BadmintonCourtManagerConsole.exe
echo.
echo ========================================

goto :end

:error
echo.
echo ========================================
echo BUILD FAILED!
echo ========================================
pause
exit /b 1

:end
pause
