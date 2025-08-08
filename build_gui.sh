#!/bin/bash

set -e  # Dừng script nếu có lỗi
set -o pipefail

# ========== Cấu hình ==========

SRC_DIR="src"
BUILD_DIR="build"
OBJ_DIR="$BUILD_DIR/obj"
INCLUDE_DIR="src/include"

# Đường dẫn wxWidgets
WXWIN="$(pwd)/wxWidgets-3.2.8"
echo "Using wxWidgets at: $WXWIN"

# Cờ biên dịch
CXX_FLAGS="-std=c++17 -w -O2"
WX_CXXFLAGS="`$WXWIN/wx-config --cxxflags`"
INCLUDE_FLAGS="-I$INCLUDE_DIR"

# Thư viện wxWidgets trên macOS
WX_LIBS="`$WXWIN/wx-config --libs`"

# ========== Kiểm tra môi trường ==========

if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ compiler not found!"
    exit 1
fi

if [ ! -d "$WXWIN/include/wx" ]; then
    echo "ERROR: wxWidgets not found at $WXWIN/include/wx"
    exit 1
fi

# ========== Dọn thư mục cũ ==========

echo "Cleaning previous build..."
rm -rf "$BUILD_DIR"
mkdir -p "$OBJ_DIR"

# ========== Biên dịch các file ==========

echo "========================================"
echo "Compiling sources..."
echo "========================================"

compile() {
    src_file="$1"
    obj_file="$2"
    echo "Compiling $src_file"
    g++ $CXX_FLAGS $INCLUDE_FLAGS $WX_CXXFLAGS -c "$src_file" -o "$obj_file"
}

# Compile models
compile "$SRC_DIR/models/User.cpp" "$OBJ_DIR/User.o"
compile "$SRC_DIR/models/Court.cpp" "$OBJ_DIR/Court.o"
compile "$SRC_DIR/models/Booking.cpp" "$OBJ_DIR/Booking.o"
compile "$SRC_DIR/models/Statistics.cpp" "$OBJ_DIR/Statistics.o"

# Compile controllers
compile "$SRC_DIR/controllers/AuthController.cpp" "$OBJ_DIR/AuthController.o"
compile "$SRC_DIR/controllers/CourtController.cpp" "$OBJ_DIR/CourtController.o"
compile "$SRC_DIR/controllers/BookingController.cpp" "$OBJ_DIR/BookingController.o"
compile "$SRC_DIR/controllers/NotificationController.cpp" "$OBJ_DIR/NotificationController.o"
compile "$SRC_DIR/controllers/StatisticsController.cpp" "$OBJ_DIR/StatisticsController.o"

# Compile patterns
compile "$SRC_DIR/patterns/BookingManager.cpp" "$OBJ_DIR/BookingManager.o"
compile "$SRC_DIR/patterns/NotificationObserver.cpp" "$OBJ_DIR/NotificationObserver.o"

# Compile utils
compile "$SRC_DIR/utils/Database.cpp" "$OBJ_DIR/Database.o"
compile "$SRC_DIR/utils/DateTimeUtils.cpp" "$OBJ_DIR/DateTimeUtils.o"

# Compile views
compile "$SRC_DIR/views/LoginFrame.cpp" "$OBJ_DIR/LoginFrame.o"
compile "$SRC_DIR/views/MainFrame.cpp" "$OBJ_DIR/MainFrame.o"
compile "$SRC_DIR/views/CourtManagementPanel.cpp" "$OBJ_DIR/CourtManagementPanel.o"
compile "$SRC_DIR/views/BookingPanel.cpp" "$OBJ_DIR/BookingPanel.o"
compile "$SRC_DIR/views/StatisticsPanel.cpp" "$OBJ_DIR/StatisticsPanel.o"
compile "$SRC_DIR/views/RegisterDialog.cpp" "$OBJ_DIR/RegisterDialog.o"
compile "$SRC_DIR/views/UserManagementPanel.cpp" "$OBJ_DIR/UserManagementPanel.o"
compile "$SRC_DIR/views/AdminPanel.cpp" "$OBJ_DIR/AdminPanel.o"

# Compile main
compile "$SRC_DIR/main.cpp" "$OBJ_DIR/main.o"

# ========== Link executable ==========

echo "========================================"
echo "Linking executable..."
echo "========================================"

g++ $CXX_FLAGS \
    $OBJ_DIR/*.o \
    $WX_LIBS \
    -o "$BUILD_DIR/BadmintonCourtManagerGUI"

echo "========================================"
echo "✅ Build completed successfully!"
echo "Executable: $BUILD_DIR/BadmintonCourtManagerGUI"
echo "========================================"
