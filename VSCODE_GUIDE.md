# VS Code Development Guide - Badminton Court Manager

## Thiết lập VS Code cho Project

### 1. Extensions cần thiết
Cài đặt các extensions sau trong VS Code:
- **C/C++** (Microsoft) - IntelliSense và debugging
- **C/C++ Extension Pack** - Bộ extensions C/C++ đầy đủ
- **Code Runner** (optional) - Chạy code nhanh
- **Better Comments** (optional) - Comments đẹp hơn

### 2. Prerequisites
- **MinGW-w64** hoặc **MSYS2** với g++ compiler
- **wxWidgets 3.1+** (nếu muốn build GUI)
- **Git** (optional)

### 3. Cài đặt MinGW-w64
```bash
# Option 1: Sử dụng winget
winget install -e --id MSYS2.MSYS2

# Option 2: Download từ https://www.mingw-w64.org/
# Option 3: Sử dụng chocolatey
choco install mingw
```

### 4. Cài đặt wxWidgets (optional cho GUI)
```bash
# Download từ https://www.wxwidgets.org/downloads/
# Extract và set WXWIN environment variable
set WXWIN=C:\wxWidgets-3.2.0
```

## Cách sử dụng trong VS Code

### 1. Mở Project
- Open Folder: `PROJECT_OOP_LAB`
- VS Code sẽ tự động detect C++ project

### 2. Build Project

#### Option A: Sử dụng VS Code Tasks (Khuyến nghị)
1. **Ctrl+Shift+P** → "Tasks: Run Task"
2. Chọn task:
   - **Build Console Version (MinGW)** - Build console app
   - **Build GUI Version (wxWidgets)** - Build GUI app (cần wxWidgets)
   - **Run Application** - Chạy app sau khi build
   - **Clean Build** - Xóa build directory
   - **Build and Run Console** - Build và chạy liền

#### Option B: Sử dụng Terminal
```bash
# Console version
./build_mingw.bat

# GUI version (cần wxWidgets)
./build_gui.bat

# Run application
./run.bat
```

#### Option C: Shortcut Keys
- **Ctrl+Shift+B** → Chọn build task
- **F5** → Debug (cần setup launch.json)

### 3. File Structure trong VS Code
```
PROJECT_OOP_LAB/
├── .vscode/
│   ├── tasks.json          # Build tasks
│   ├── c_cpp_properties.json (auto-generated)
│   └── settings.json       (auto-generated)
├── src/
│   ├── include/            # Headers (.h files)
│   ├── models/             # Data models
│   ├── controllers/        # Business logic
│   ├── views/              # GUI components
│   ├── patterns/           # Design patterns
│   ├── utils/              # Utilities
│   ├── main.cpp            # GUI main
│   └── console_main.cpp    # Console main
├── build/                  # Build output
├── build_mingw.bat        # Console build script
├── build_gui.bat          # GUI build script
└── run.bat                # Run script
```

### 4. IntelliSense Configuration
VS Code sẽ tự động tạo `c_cpp_properties.json`:
```json
{
    "configurations": [
        {
            "name": "Win32",
            "includePath": [
                "${workspaceFolder}/**",
                "${workspaceFolder}/src/include"
            ],
            "defines": [
                "_DEBUG",
                "UNICODE",
                "_UNICODE"
            ],
            "compilerPath": "C:/mingw64/bin/g++.exe",
            "cStandard": "c17",
            "cppStandard": "c++17",
            "intelliSenseMode": "windows-gcc-x64"
        }
    ],
    "version": 4
}
```

### 5. Debugging Setup
Tạo `.vscode/launch.json`:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Console App",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/BadmintonCourtManager.exe",
            "args": [],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true,
            "MIMode": "gdb",
            "miDebuggerPath": "C:/mingw64/bin/gdb.exe",
            "preLaunchTask": "Build Console Version (MinGW)"
        }
    ]
}
```

## Workflow Development

### 1. Daily Development
1. **Ctrl+Shift+P** → "Tasks: Run Task" → "Build Console Version"
2. Test functionality với console version
3. Edit code trong VS Code với IntelliSense
4. **Ctrl+Shift+B** để build nhanh
5. **./run.bat** để test

### 2. Debugging
1. Set breakpoints bằng cách click vào line numbers
2. **F5** để start debugging
3. **F10** - Step over, **F11** - Step into, **Shift+F11** - Step out
4. **Ctrl+Shift+F5** - Restart debugging

### 3. Code Navigation
- **Ctrl+Click** - Go to definition
- **F12** - Go to definition
- **Shift+F12** - Find all references
- **Ctrl+T** - Go to symbol
- **Ctrl+Shift+O** - Go to symbol in file

### 4. Code Completion
- **Ctrl+Space** - Trigger IntelliSense
- **Ctrl+Shift+Space** - Parameter hints
- **Ctrl+.** - Quick fix

## Tips & Tricks

### 1. Terminal Integration
- **Ctrl+`** - Mở terminal tích hợp
- Có thể chạy build commands trực tiếp trong terminal

### 2. File Operations
- **Ctrl+N** - New file
- **Ctrl+Shift+N** - New window
- **Ctrl+P** - Quick open file

### 3. Multiple Cursors
- **Alt+Click** - Add cursor
- **Ctrl+Alt+Down/Up** - Add cursor below/above
- **Ctrl+D** - Select next occurrence

### 4. Code Formatting
- **Shift+Alt+F** - Format document
- **Ctrl+K Ctrl+F** - Format selection

### 5. Git Integration
- **Ctrl+Shift+G** - Open Git panel
- Built-in Git support for version control

## Troubleshooting

### 1. g++ not found
```bash
# Add MinGW to PATH
set PATH=%PATH%;C:\mingw64\bin
```

### 2. IntelliSense errors
- **Ctrl+Shift+P** → "C/C++: Reset IntelliSense Database"
- Check `c_cpp_properties.json` configuration

### 3. Build errors
- Check terminal output trong VS Code
- Verify all source files exist
- Check include paths

### 4. wxWidgets not found (GUI version)
```bash
# Set WXWIN environment variable
set WXWIN=C:\path\to\wxWidgets
```

### 5. Permission errors
- Run VS Code as Administrator if needed
- Check file permissions in project folder

## Performance Tips

### 1. Large Projects
- Use workspace settings để optimize IntelliSense
- Exclude build directories from file watching

### 2. Memory Usage
- Close unused tabs
- Use "outline" view để navigate large files

Với setup này, bạn có thể develop C++ project một cách hiệu quả trong VS Code với full IntelliSense, debugging, và build integration!
