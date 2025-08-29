#!/bin/bash
set -e  # Dừng khi có lỗi
set -o pipefail

# ========== Cấu hình ==========
SRC_DIR="src"
BUILD_DIR="build"
OBJ_DIR="$BUILD_DIR/obj"
INCLUDE_DIR="src/include"

echo "========================================"
echo " Building Console Version"
echo "========================================"

# Kiểm tra g++
if ! command -v g++ &> /dev/null; then
    echo "ERROR: g++ compiler not found!"
    echo "Please install g++ (sudo apt install g++)"
    exit 1
fi

# Compiler flags
CXX_FLAGS="-std=c++17 -O2"
INCLUDE_FLAGS="-I$INCLUDE_DIR"

# Clean build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Cleaning existing build directory..."
    rm -rf "$BUILD_DIR"
fi

if [ -d "data" ]; then
    echo "Cleaning existing data directory..."
    rm -rf "data"
fi

# Create build directory
mkdir -p "$OBJ_DIR"

echo
echo "Compiler:"
g++ --version
echo
echo "========================================"
echo " Compiling source files..."
echo "========================================"

# Compile models
echo "Compiling models..."
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/models/User.cpp" -o "$OBJ_DIR/User.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/models/Court.cpp" -o "$OBJ_DIR/Court.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/models/Booking.cpp" -o "$OBJ_DIR/Booking.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/models/Statistics.cpp" -o "$OBJ_DIR/Statistics.o"

# Compile controllers
echo "Compiling controllers..."
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/controllers/AuthController.cpp" -o "$OBJ_DIR/AuthController.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/controllers/CourtController.cpp" -o "$OBJ_DIR/CourtController.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/controllers/BookingController.cpp" -o "$OBJ_DIR/BookingController.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/controllers/StatisticsController.cpp" -o "$OBJ_DIR/StatisticsController.o"

# Compile patterns
echo "Compiling design patterns..."
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/patterns/BookingManager.cpp" -o "$OBJ_DIR/BookingManager.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/patterns/NotificationObserver.cpp" -o "$OBJ_DIR/NotificationObserver.o"

# Compile utilities
echo "Compiling utilities..."
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/utils/Database.cpp" -o "$OBJ_DIR/Database.o"
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/utils/DateTimeUtils.cpp" -o "$OBJ_DIR/DateTimeUtils.o"

# Compile console main
echo "Compiling console main application..."
g++ $CXX_FLAGS $INCLUDE_FLAGS -c "$SRC_DIR/console_main.cpp" -o "$OBJ_DIR/console_main.o"

echo
echo "========================================"
echo " Linking console executable..."
echo "========================================"

# Link
g++ $CXX_FLAGS \
    "$OBJ_DIR/User.o" \
    "$OBJ_DIR/Court.o" \
    "$OBJ_DIR/Booking.o" \
    "$OBJ_DIR/Statistics.o" \
    "$OBJ_DIR/AuthController.o" \
    "$OBJ_DIR/CourtController.o" \
    "$OBJ_DIR/BookingController.o" \
    "$OBJ_DIR/StatisticsController.o" \
    "$OBJ_DIR/BookingManager.o" \
    "$OBJ_DIR/NotificationObserver.o" \
    "$OBJ_DIR/Database.o" \
    "$OBJ_DIR/DateTimeUtils.o" \
    "$OBJ_DIR/console_main.o" \
    -o "$BUILD_DIR/BadmintonCourtManagerConsole"

echo
echo "========================================"
echo " Console Build completed successfully!"
echo "========================================"
echo "Executable: $BUILD_DIR/BadmintonCourtManagerConsole"
echo
echo "To run the console application:"
echo "  cd build"
echo "  ./BadmintonCourtManagerConsole"
echo
echo "========================================"
