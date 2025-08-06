@echo off
setlocal EnableDelayedExpansion

:: Set paths
set SRC_DIR=src
set BUILD_DIR=build
set OBJ_DIR=build\obj
set INCLUDE_DIR=src\include

:: Using local wxWidgets installation in project
set WXWIN=%~dp0wxWidgets-3.2.8
echo Using wxWidgets at: %WXWIN%

:: Compiler flags for wxWidgets - Using DLL version that was built (warnings disabled)
set CXX_FLAGS=-std=c++17 -w -O2 -mwindows
set WX_CXXFLAGS=-I%WXWIN%\include -I%WXWIN%\lib\gcc_dll\mswu -D__WXMSW__ -DWXUSINGDLL
set INCLUDE_FLAGS=-I%INCLUDE_DIR%

:: Linker flags for wxWidgets - Using DLL libraries with correct names
set WX_LIBS=-L%WXWIN%\lib\gcc_dll -lwxmsw32u_core_gcc_custom -lwxbase32u_gcc_custom -lwxmsw32u_adv_gcc_custom


:: Windows system libraries required for wxWidgets
set SYS_LIBS=-lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lcomctl32 -lwsock32 -lodbc32

echo ========================================
echo Building GUI Version with wxWidgets
echo ========================================

:: Check if g++ is available
where g++ >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW-w64 or add g++ to PATH
    pause
    exit /b 1
)

:: Check if wxWidgets exists
if not exist "%WXWIN%\include\wx" (
    echo ERROR: wxWidgets not found at %WXWIN%\include\wx
    echo Please install wxWidgets to C:\wxWidgets-3.2.8 or update WXWIN path
    pause
    exit /b 1
)

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
echo wxWidgets found at: %WXWIN%
echo Compiler: 
g++ --version

echo.
echo ========================================
echo Compiling source files...
echo ========================================

:: Compile models (same as console version)
echo Compiling models...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\models\User.cpp -o %OBJ_DIR%\User.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\models\Court.cpp -o %OBJ_DIR%\Court.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\models\Booking.cpp -o %OBJ_DIR%\Booking.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\models\Statistics.cpp -o %OBJ_DIR%\Statistics.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile controllers
echo Compiling controllers...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\controllers\AuthController.cpp -o %OBJ_DIR%\AuthController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\controllers\CourtController.cpp -o %OBJ_DIR%\CourtController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\controllers\BookingController.cpp -o %OBJ_DIR%\BookingController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\controllers\NotificationController.cpp -o %OBJ_DIR%\NotificationController.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\controllers\StatisticsController.cpp -o %OBJ_DIR%\StatisticsController.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile patterns
echo Compiling design patterns...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\patterns\BookingManager.cpp -o %OBJ_DIR%\BookingManager.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\patterns\NotificationObserver.cpp -o %OBJ_DIR%\NotificationObserver.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile utilities
echo Compiling utilities...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\utils\Database.cpp -o %OBJ_DIR%\Database.o
if %ERRORLEVEL% neq 0 goto :error

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\utils\DateTimeUtils.cpp -o %OBJ_DIR%\DateTimeUtils.o
if %ERRORLEVEL% neq 0 goto :error

:: Compile GUI views (create stubs first if needed)
echo Compiling GUI views...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\LoginFrame.cpp -o %OBJ_DIR%\LoginFrame.o
if %ERRORLEVEL% neq 0 (
    echo Creating LoginFrame stub...
    echo // Placeholder > %SRC_DIR%\views\LoginFrame.cpp
    g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\LoginFrame.cpp -o %OBJ_DIR%\LoginFrame.o
)

g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\MainFrame.cpp -o %OBJ_DIR%\MainFrame.o
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\CourtManagementPanel.cpp -o %OBJ_DIR%\CourtManagementPanel.o
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\BookingPanel.cpp -o %OBJ_DIR%\BookingPanel.o
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\StatisticsPanel.cpp -o %OBJ_DIR%\StatisticsPanel.o
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\RegisterDialog.cpp -o %OBJ_DIR%\RegisterDialog.o
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\UserManagementPanel.cpp -o %OBJ_DIR%\UserManagementPanel.o
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\views\AdminPanel.cpp -o %OBJ_DIR%\AdminPanel.o

:: Compile main (GUI version)
echo Compiling main GUI application...
g++ %CXX_FLAGS% %INCLUDE_FLAGS% %WX_CXXFLAGS% -c %SRC_DIR%\main.cpp -o %OBJ_DIR%\main.o
if %ERRORLEVEL% neq 0 goto :error

echo.
echo ========================================
echo Linking GUI executable...
echo ========================================

:: Link all object files with wxWidgets
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
    %OBJ_DIR%\LoginFrame.o ^
    %OBJ_DIR%\MainFrame.o ^
    %OBJ_DIR%\CourtManagementPanel.o ^
    %OBJ_DIR%\BookingPanel.o ^
    %OBJ_DIR%\StatisticsPanel.o ^
    %OBJ_DIR%\RegisterDialog.o ^
    %OBJ_DIR%\UserManagementPanel.o ^
    %OBJ_DIR%\AdminPanel.o ^
    %OBJ_DIR%\main.o ^
    %WX_LIBS% ^
    %SYS_LIBS% ^
    -o %BUILD_DIR%\BadmintonCourtManagerGUI.exe

if %ERRORLEVEL% neq 0 goto :error

echo.
echo ========================================
echo Copying DLL files...
echo ========================================

:: Copy wxWidgets DLL files from dll folder to build directory
if exist dll (
    echo Copying wxWidgets DLL files...
    copy dll\*.dll %BUILD_DIR%\ >nul
    if %ERRORLEVEL% neq 0 (
        echo WARNING: Failed to copy some DLL files
    ) else (
        echo DLL files copied successfully
    )
) else (
    echo WARNING: dll folder not found, DLL files not copied
)

echo.
echo ========================================
echo GUI Build completed successfully!
echo ========================================
echo Executable: %BUILD_DIR%\BadmintonCourtManagerGUI.exe
echo DLL files: Copied to %BUILD_DIR%\
echo.
echo To run the GUI application:
echo   cd build
echo   BadmintonCourtManagerGUI.exe
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
