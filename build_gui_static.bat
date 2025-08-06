@echo off
setlocal EnableDelayedExpansion

:: Set paths
set SRC_DIR=src
set BUILD_DIR=build
set OBJ_DIR=build\obj
set INCLUDE_DIR=src\include

:: Using wxWidgets installation at C:\wxWidgets-3.2.8 (static linking)
set WXWIN=C:\wxWidgets-3.2.8
echo Using wxWidgets at: %WXWIN%

:: Compiler flags for wxWidgets (static linking)
set CXX_FLAGS=-std=c++17 -Wall -O2 -mwindows -static
set WX_CXXFLAGS=-I%WXWIN%\include -I%WXWIN%\lib\gcc_lib\mswu -D__WXMSW__ -DUSE_STATIC_WX
set INCLUDE_FLAGS=-I%INCLUDE_DIR%

:: Linker flags for wxWidgets - Static linking (simpler setup)
set WX_LIBS=-L%WXWIN%\lib\gcc_lib -lwxmsw32u_core -lwxbase32u -lwxmsw32u_adv

:: Windows system libraries required for wxWidgets
set SYS_LIBS=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lcomctl32 -lwsock32 -lodbc32

echo ========================================
echo Building GUI Version with wxWidgets (Static)
echo ========================================

:: Check if g++ is available
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW-w64 or add g++ to PATH
    pause
    exit /b 1
)

:: Check if wxWidgets source exists (for now)
if not exist "%WXWIN%\include\wx" (
    echo ERROR: wxWidgets not found at %WXWIN%\include\wx
    echo Please install wxWidgets to C:\wxWidgets-3.2.8
    pause
    exit /b 1
)

:: Create build directory
if not exist build mkdir build
if not exist build\obj mkdir build\obj

echo.
echo wxWidgets found at: %WXWIN%
echo Compiler: 
g++ --version

echo.
echo ========================================
echo Compiling source files (without GUI for now)...
echo ========================================

:: Compile models (same as console version)
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

g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\controllers\NotificationController.cpp -o %OBJ_DIR%\NotificationController.o
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

:: Skip GUI compilation for now until wxWidgets is built
echo.
echo Skipping GUI compilation until wxWidgets is properly built...
echo For now, building console version with all business logic.

:: Use console main for now
echo Compiling console main...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% -c %SRC_DIR%\console_main.cpp -o %OBJ_DIR%\console_main.o
if %ERRORLEVEL% neq 0 goto :error

echo.
echo ========================================
echo Linking Console executable...
echo ========================================

:: Link console version
g++ %CXX_FLAGS% ^
    %OBJ_DIR%\User.o ^
    %OBJ_DIR%\Court.o ^
    %OBJ_DIR%\Booking.o ^
    %OBJ_DIR%\Statistics.o ^
    %OBJ_DIR%\AuthController.o ^
    %OBJ_DIR%\CourtController.o ^
    %OBJ_DIR%\BookingController.o ^
    %OBJ_DIR%\NotificationController.o ^
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
echo NOTE: GUI version will be available once wxWidgets build completes.
echo      Run the wxWidgets build: cd C:\wxWidgets-3.2.8\build\msw
echo                               mingw32-make -f makefile.gcc SHARED=1 UNICODE=1 BUILD=release
echo ========================================

goto :end

:error
echo.
echo ========================================
echo BUILD FAILED!
echo ========================================
echo Check the error messages above.
pause
exit /b 1

:end
pause
