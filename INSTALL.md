# Installation and Setup Guide

## Prerequisites

### Required Software
1. **Microsoft Visual Studio 2019/2022** with C++ development tools
2. **CMake 3.16 or later**
3. **wxWidgets 3.1 or later**

### Installing wxWidgets

#### Option 1: vcpkg (Recommended)
```bash
# Install vcpkg if not already installed
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Install wxWidgets
.\vcpkg install wxwidgets:x64-windows
```

#### Option 2: Manual Installation
1. Download wxWidgets from https://www.wxwidgets.org/downloads/
2. Extract to `C:\wxWidgets`
3. Open `C:\wxWidgets\build\msw\wx_vc16.sln` in Visual Studio
4. Build both Debug and Release configurations for x64

## Building the Project

### Using the Build Script (Recommended)
1. Open Command Prompt as Administrator
2. Navigate to the project directory
3. Run: `build.bat`

### Manual Build
1. Create build directory: `mkdir build && cd build`
2. Generate project files: `cmake .. -G "Visual Studio 17 2022"`
3. Build the project: `cmake --build . --config Release`

## Running the Application

1. Navigate to `build\Release\`
2. Run `BadmintonCourtManager.exe`

### Default Login Credentials
- **Admin**: admin@badminton.com / admin123
- **Customer**: Register new account from login screen

## Project Structure

```
PROJECT_OOP_LAB/
├── src/
│   ├── include/          # Header files
│   ├── models/           # Data models (User, Court, Booking)
│   ├── controllers/      # Business logic controllers
│   ├── views/           # GUI components (wxWidgets)
│   ├── patterns/        # Design pattern implementations
│   ├── utils/           # Utility classes
│   └── main.cpp         # Application entry point
├── build/               # Build output directory
├── CMakeLists.txt       # CMake configuration
├── build.bat           # Windows build script
└── README.md           # Project documentation
```

## Features

### Implemented
- ✅ User authentication (Admin/Customer roles)
- ✅ Court management (CRUD operations)
- ✅ Booking system with conflict detection
- ✅ Notification system (Observer pattern)
- ✅ Singleton booking manager
- ✅ Basic GUI framework

### To Be Implemented
- 🔄 Complete GUI panels
- 🔄 Statistics and reporting
- 🔄 Email notifications
- 🔄 Data persistence (file/database)
- 🔄 Advanced booking features

## Troubleshooting

### Common Issues

1. **wxWidgets not found**
   - Ensure wxWidgets is properly installed
   - Check CMake can find wxWidgets: `find_package(wxWidgets)`

2. **Build errors**
   - Verify Visual Studio C++ tools are installed
   - Check CMake version compatibility

3. **Runtime errors**
   - Ensure all DLL dependencies are available
   - Check working directory contains data files

### Support
For technical support, please check the project documentation or contact the development team.
