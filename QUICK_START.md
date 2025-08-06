# Hướng dẫn Build và Run Project trong VS Code

## 🚀 Bạn đã có đầy đủ các file cần thiết!

### 📁 Structure đã hoàn thiện:
```
PROJECT_OOP_LAB/
├── src/                    # Source code
├── .vscode/tasks.json      # VS Code tasks
├── build_mingw.bat        # Console build script  
├── build_gui.bat          # GUI build script (cần wxWidgets)
├── run.bat                # Run script
└── VSCODE_GUIDE.md        # Chi tiết guide
```

## 🔧 Cách build trong VS Code:

### Method 1: Sử dụng VS Code Tasks (KHUYẾN NGHỊ)
1. **Ctrl+Shift+P** 
2. Gõ: "Tasks: Run Task"
3. Chọn: **"Build Console Version (MinGW)"**
4. Hoặc **Ctrl+Shift+B** → chọn build task

### Method 2: Sử dụng Terminal trong VS Code
1. **Ctrl+`** (mở terminal)
2. Chạy lệnh:
```bash
# Console version
.\build_mingw.bat

# GUI version (nếu có wxWidgets)
.\build_gui.bat

# Chạy app
.\run.bat
```

### Method 3: Click và chạy
- Double-click vào `build_mingw.bat` trong File Explorer
- Sau đó double-click `run.bat`

## ⚡ Quick Start:

### 1. Mở project trong VS Code:
```bash
# Mở VS Code
code .
# hoặc
code PROJECT_OOP_LAB/
```

### 2. Build ngay lập tức:
- **Ctrl+Shift+B** 
- Chọn "Build Console Version (MinGW)"
- Đợi build hoàn thành

### 3. Run application:
- **Ctrl+Shift+P** → "Tasks: Run Task" → "Run Application"
- Hoặc trong terminal: `.\run.bat`

## 🎯 Features sẵn có:

### ✅ Console Application
- ✅ User authentication system
- ✅ Court management (CRUD)
- ✅ Booking system với conflict detection
- ✅ Notification system (Observer pattern)
- ✅ Singleton BookingManager
- ✅ Statistics framework

### 🔧 Development Tools
- ✅ VS Code IntelliSense support
- ✅ Build tasks cho VS Code
- ✅ Problem matcher cho error detection
- ✅ Terminal integration

### 📋 Default Data
- **Admin account**: admin@badminton.com / admin123
- **3 courts** được tạo tự động
- **Notification system** sẵn sàng

## 🎮 Cách sử dụng Console App:

Khi chạy, bạn sẽ thấy menu:
```
=== MAIN MENU ===
1. Login/Authentication Test
2. Court Management Test  
3. Booking System Test
4. Statistics Test
0. Exit
```

### Test Flow:
1. **Chọn 1** → Login với admin@badminton.com / admin123
2. **Chọn 2** → Xem courts, test thêm court mới
3. **Chọn 3** → Test booking system và notifications
4. **Chọn 4** → Xem statistics

## 🐛 Troubleshooting:

### "g++ not found"
```bash
# Cài MinGW-w64
winget install -e --id MSYS2.MSYS2
# hoặc download từ: https://www.mingw-w64.org/
```

### Build errors
- Kiểm tra tất cả files trong `/src/` folder
- Run "Clean Build" task trước khi build lại

### VS Code issues
- **Ctrl+Shift+P** → "Developer: Reload Window"
- Restart VS Code

## 🚀 Next Steps:

### Để phát triển tiếp:
1. **Hoàn thiện GUI**: Implement wxWidgets panels
2. **Database**: Thêm SQLite cho data persistence  
3. **Advanced features**: Payment, advanced booking
4. **Testing**: Unit tests cho các components

### Để build GUI version:
1. Cài wxWidgets
2. Set WXWIN environment variable
3. Run `.\build_gui.bat`

---

**🎉 Chúc mừng! Project đã sẵn sàng để development trong VS Code!**

Để bắt đầu:
1. Mở VS Code
2. **Ctrl+Shift+B** 
3. **Run Application**
4. Enjoy coding! 🚀
