# Badminton Court Management System

A comprehensive desktop application for managing badminton court facilities, built with C++ and wxWidgets. This system provides complete functionality for court management, user administration, booking management, and detailed analytics.

## Features

### Core Functionality
- **User Management**: Admin and regular user roles with authentication
- **Court Management**: Add, edit, and manage court availability and pricing
- **Booking System**: Real-time booking with time slot management
- **Statistics & Analytics**: Comprehensive reporting with export capabilities
- **Data Persistence**: File-based storage for users, courts, and bookings

### User Interface
- Modern, dark-themed GUI for Windows, Linux, and macOS
- Intuitive navigation with tabbed interface
- Real-time data updates and validation
- Responsive design with proper error handling

### Additional Features
- **Console Mode**: Optional command-line interface for terminal-based operation

### Advanced Features
- **Export Functionality**: CSV export for statistics and reports
- **Notification System**: Observer pattern for booking notifications
- **Status Tracking**: Complete booking lifecycle management
- **Revenue Analytics**: Daily, weekly, and monthly revenue reports
- **Court Utilization**: Usage statistics and performance metrics

## Technical Architecture

### Design Patterns
- **MVC Architecture**: Clean separation of Model, View, and Controller
- **Observer Pattern**: For notification system
- **Singleton Pattern**: For booking manager
- **Factory Pattern**: For object creation

### Technologies
- **Language**: C++17
- **GUI Framework**: wxWidgets 3.2.8
- **Platform**: Windows, Linux, macOS (cross-platform, Apple Silicon & Intel supported)
- **Build System**: Custom shell script with g++

## Prerequisites

### System Requirements
- Windows 10 or later / Ubuntu 20.04+ or other modern Linux / macOS 11.0 or later
- Intel or Apple Silicon (arm64) processor
- (macOS) Xcode Command Line Tools, Homebrew (for dependencies), wxWidgets
- (Linux) build-essential, libwxgtk3.2-dev (wxWidgets), libpcre2-dev
- (Windows) MinGW or MSVC, wxWidgets

### Dependencies
- wxWidgets 3.2.8 (included in project)
- pcre2 (for regex support)
- Standard C++ libraries


## Installation & Setup

### 1. Clone the Repository (All Platforms)
```bash
git clone https://github.com/ndeshi06/PROJECT_OOP_LAB.git
cd PROJECT_OOP_LAB
```

---

### Windows
1. **Install Dependencies:**
  - Download and install [wxWidgets 3.2.8](https://www.wxwidgets.org/downloads/) (not required, already included in the project folder).
   - Ensure you have a C++ compiler (MinGW recommended).
2. **Build the Application:**
   - Double-click `build_gui.bat` or run in Command Prompt:
  ```cmd
  build_gui.bat
  ```
3. **Run the Application:**
   - Navigate to the `build` folder and double-click `BadmintonCourtManagerGUI.exe`.

---

### Linux
1. **Install Dependencies:**
  - Install build tools:
  ```bash
  sudo apt update
  sudo apt install build-essential libpcre2-dev libwxgtk3.2-dev
  ```
2. **Build the Application:**
  ```bash
  chmod +x build_gui_linux.sh
  ./build_gui_linux.sh
  ```
3. **Run the Application:**
  ```bash
  cd build
  ./BadmintonCourtManagerGUI
  ```

---

### macOS
1. **Install Dependencies:**
   - Install [Homebrew](https://brew.sh/) if not already installed:
  ```bash
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
  brew install pcre2
  ```
2. **Build wxWidgets (not required, already built by the author):**
  > **Note:** You can skip this step because wxWidgets is already pre-built in the project folder. **Only perform this if you encounter errors related to the wxWidgets library or if you want to rebuild wxWidgets yourself.**
  ```bash
  cd wxWidgets-3.2.8
  ./configure --enable-unicode --disable-shared --enable-static --with-osx_cocoa
  make -j4
  cd ..
  ```
3. **Build the Application:**
  ```bash
  chmod +x build_gui_mac.sh
  ./build_gui_mac.sh
  ```
4. **Run the Application:**
  ```bash
  cd build
  ./BadmintonCourtManagerGUI
  ```

## Usage Guide

### Initial Setup
1. **First Launch**: The system creates default admin user and sample courts
2. **Login**: Use admin credentials to access full functionality
3. **Configuration**: Set up courts, pricing, and user accounts

### Default Credentials
- **Email**: `admin@badminton.com`
- **Password**: `admin123`

### Main Features

#### Court Management
- Navigate to "Court Management" tab
- Add new courts with pricing and descriptions
- Edit existing court details
- Set court status (Available, Maintenance, Out of Service)

#### Booking Management
- View "Booking History" for all reservations
- Create new bookings with date/time selection
- Track booking status (Pending, Confirmed, Cancelled, Completed)
- Manage booking conflicts and validation

#### User Administration
- Access "User Management" for user accounts
- Create new user accounts
- Assign roles and permissions
- View user activity and booking history

#### Statistics & Reports
- Visit "Statistics" tab for comprehensive analytics
- Select date ranges for custom reports
- Export data to CSV format for external analysis
- View revenue trends and court utilization metrics

## Alternative Console Interface

For users who prefer command-line interfaces or need to run the application in terminal environments, an optional console version is available.

### Building Console Version
```cmd
build_console.bat
```

### Running Console Application
```cmd
cd build
BadmintonCourtManagerConsole.exe
```

The console interface provides the same core functionality as the GUI version through menu-driven navigation.

## 📊 Data Export

The system supports comprehensive data export functionality:

### Export Format
- **CSV Files**: Compatible with Excel and other spreadsheet applications
- **Report Sections**: Summary, Court Usage, Daily Breakdown
- **Custom Date Ranges**: Flexible reporting periods

### Export Content
- Total bookings and revenue
- Court-wise utilization statistics
- Daily breakdown with detailed metrics
- Usage rates and performance indicators

## Project Structure

```
PROJECT_OOP_LAB/
├── src/
│   ├── main.cpp             # Main GUI application entry point
│   ├── console_main.cpp     # Optional console interface
│   ├── controllers/         # Business logic controllers
│   ├── models/              # Data models and entities
│   ├── views/               # GUI components and panels
│   ├── patterns/            # Design pattern implementations
│   ├── utils/               # Utility classes and helpers
│   └── include/             # Header files
├── build/                   # Compiled executables and data files
├── dll/                     # DLL files (for Windows)
├── wxWidgets-3.2.8/         # wxWidgets framework
├── build_gui.bat            # Main GUI build script for Windows
├── build_console.bat        # Optional console build script (Windows)
├── build_console.sh         # Optional console build script (Linux/macOS)
├── build_gui.sh             # GUI build script (Windows)
├── build_gui_linux.sh       # GUI build script for Linux
├── build_gui_mac.sh         # GUI build script for macOS
└── README.md                # This file
```

## Authors
  This application is developed by Group 8 - 24C06.
### Contributors
- **Vo Nguyen Khoa** - *Feature development, code review* - [GitHub Profile](https://github.com/nguyenkhoa838)
- **Tran Gia Phuc** - *code review, testing, documentation* - [GitHub Profile](https://github.com/tgphuc1901)
- **Trinh Tan Phat** - *Bug fixes, documentation* - [GitHub Profile](https://github.com/ttphat-fitus)
- **Do Duc Thinh** - *Initial development, GUI design, feature development* - [GitHub Profile](https://github.com/ndeshi06)
- **Chau Vu Trung** - *GUI design, feature development* - [GitHub Profile](https://github.com/CVTrung)
