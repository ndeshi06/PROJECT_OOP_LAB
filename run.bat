@echo off
echo ========================================
echo Running Badminton Court Manager
echo ========================================

if exist build\BadmintonCourtManager.exe (
    echo Starting console version...
    cd build
    BadmintonCourtManager.exe
    cd ..
) else if exist build\BadmintonCourtManagerGUI.exe (
    echo Starting GUI version...
    cd build
    BadmintonCourtManagerGUI.exe
    cd ..
) else (
    echo No executable found!
    echo Please build the project first:
    echo   - For console version: build_mingw.bat
    echo   - For GUI version: build_gui.bat
    pause
)

echo.
echo ========================================
)

echo Starting application...
echo.
cd build
BadmintonCourtManager.exe

echo.
echo Application finished.
pause
