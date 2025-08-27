#!/bin/bash
set -e  # dừng ngay nếu có lỗi

# ====== Đường dẫn ======
SRC_DIR="src"
BUILD_DIR="build"
OBJ_DIR="$BUILD_DIR/obj"
INCLUDE_DIR="$SRC_DIR/include"

# ====== Cờ compiler ======
CXX="g++"
CXX_FLAGS="-std=c++17 -O2"
WX_FLAGS=$(wx-config --cxxflags)
WX_LIBS=$(wx-config --libs)

INCLUDE_FLAGS="-I$INCLUDE_DIR"

echo "========================================"
echo " Building GUI Version with wxWidgets"
echo "========================================"

# Kiểm tra g++
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ compiler not found!"
    exit 1
fi

# Kiểm tra wxWidgets
if ! wx-config --version &> /dev/null; then
    echo "ERROR: wxWidgets not found!"
    echo "Please install with: sudo apt install libwxgtk3.2-dev"
    exit 1
fi

# Xoá build cũ
rm -rf "$BUILD_DIR"
mkdir -p "$OBJ_DIR"

echo
echo "wxWidgets version: $(wx-config --version)"
echo "Compiler: $($CXX --version | head -n 1)"

echo
echo "========================================"
echo " Compiling source files..."
echo "========================================"

# Hàm compile 1 file
compile_file() {
    SRC=$1
    OBJ=$2
    echo "Compiling $SRC..."
    $CXX $CXX_FLAGS $INCLUDE_FLAGS $WX_FLAGS -c "$SRC" -o "$OBJ"
}

# Models
for f in User Court Booking Statistics; do
    compile_file "$SRC_DIR/models/$f.cpp" "$OBJ_DIR/$f.o"
done

# Controllers
for f in AuthController CourtController BookingController StatisticsController; do
    compile_file "$SRC_DIR/controllers/$f.cpp" "$OBJ_DIR/$f.o"
done

# Patterns
for f in BookingManager NotificationObserver; do
    compile_file "$SRC_DIR/patterns/$f.cpp" "$OBJ_DIR/$f.o"
done

# Utils
for f in Database DateTimeUtils; do
    compile_file "$SRC_DIR/utils/$f.cpp" "$OBJ_DIR/$f.o"
done

# Views
for f in LoginFrame MainFrame CourtManagementPanel BookingPanel StatisticsPanel RegisterDialog UserManagementPanel AdminPanel; do
    compile_file "$SRC_DIR/views/$f.cpp" "$OBJ_DIR/$f.o"
done

# Main
compile_file "$SRC_DIR/main.cpp" "$OBJ_DIR/main.o"

echo
echo "========================================"
echo " Linking GUI executable..."
echo "========================================"

$CXX $CXX_FLAGS $OBJ_DIR/*.o $WX_LIBS -o $BUILD_DIR/BadmintonCourtManagerGUI

echo
echo "========================================"
echo " GUI Build completed successfully!"
echo "========================================"
echo "Executable: $BUILD_DIR/BadmintonCourtManagerGUI"
echo
echo "To run the GUI application:"
echo "  cd build"
echo "  ./BadmintonCourtManagerGUI"
echo "========================================"
