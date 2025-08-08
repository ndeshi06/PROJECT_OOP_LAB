# Badminton Court Management System

A comprehensive desktop application for managing badminton court facilities, built with C++ and wxWidgets. This system provides complete functionality for court management, user administration, booking management, and detailed analytics.

## 🏸 Features

### Core Functionality
- **User Management**: Admin and regular user roles with authentication
- **Court Management**: Add, edit, and manage court availability and pricing
- **Booking System**: Real-time booking with time slot management
- **Statistics & Analytics**: Comprehensive reporting with export capabilities
- **Data Persistence**: File-based storage for users, courts, and bookings

### User Interface
- Modern, dark-themed GUI optimized for macOS
- Intuitive navigation with tabbed interface
- Real-time data updates and validation
- Responsive design with proper error handling

### Advanced Features
- **Export Functionality**: CSV export for statistics and reports
- **Notification System**: Observer pattern for booking notifications
- **Status Tracking**: Complete booking lifecycle management
- **Revenue Analytics**: Daily, weekly, and monthly revenue reports
- **Court Utilization**: Usage statistics and performance metrics

## 🛠️ Technical Architecture

### Design Patterns
- **MVC Architecture**: Clean separation of Model, View, and Controller
- **Observer Pattern**: For notification system
- **Singleton Pattern**: For booking manager
- **Factory Pattern**: For object creation

### Technologies
- **Language**: C++17
- **GUI Framework**: wxWidgets 3.2.8
- **Platform**: macOS (Apple Silicon optimized)
- **Build System**: Custom shell script with g++

## 📋 Prerequisites

### System Requirements
- macOS 11.0 or later
- Apple Silicon (arm64) or Intel processor
- Xcode Command Line Tools
- Homebrew (for dependencies)

### Dependencies
- wxWidgets 3.2.8 (included in project)
- pcre2 (for regex support)
- Standard C++ libraries

## 🚀 Installation & Setup

### 1. Clone the Repository
```bash
git clone https://github.com/ndeshi06/PROJECT_OOP_LAB.git
cd PROJECT_OOP_LAB
```

### 2. Install System Dependencies
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install pcre2 for arm64
brew install pcre2
```

### 3. Configure wxWidgets
```bash
cd wxWidgets-3.2.8
./configure --enable-unicode --disable-shared --enable-static --with-osx_cocoa
make -j4
cd ..
```

### 4. Build the Application
```bash
chmod +x build_gui.sh
./build_gui.sh
```

### 5. Run the Application
```bash
cd build
./BadmintonCourtManagerGUI
```

## 📖 Usage Guide

### Initial Setup
1. **First Launch**: The system creates default admin user and sample courts
2. **Login**: Use admin credentials to access full functionality
3. **Configuration**: Set up courts, pricing, and user accounts

### Default Credentials
- **Username**: `admin`
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

## 🏗️ Project Structure

```
PROJECT_OOP_LAB/
├── src/
│   ├── controllers/          # Business logic controllers
│   ├── models/              # Data models and entities
│   ├── views/               # GUI components and panels
│   ├── patterns/            # Design pattern implementations
│   ├── utils/               # Utility classes and helpers
│   └── include/             # Header files
├── build/                   # Build output directory
├── data/                    # Application data files
├── wxWidgets-3.2.8/         # wxWidgets framework
├── build_gui.sh            # Build script
└── README.md               # This file
```

## 🔧 Development

### Building from Source
```bash
# Clean build
rm -rf build/*
./build_gui.sh

# Debug build (add debug flags to build_gui.sh)
g++ -g -DDEBUG [other flags...]
```

### Code Style
- Follow C++17 standards
- Use meaningful variable and function names
- Implement proper error handling
- Document complex algorithms and business logic

### Adding New Features
1. Create appropriate model classes in `src/models/`
2. Implement controllers in `src/controllers/`
3. Design GUI components in `src/views/`
4. Update build script if new files are added

## 🐛 Troubleshooting

### Common Issues

#### Build Errors
- **wxWidgets not found**: Ensure wxWidgets is properly configured and built
- **Architecture mismatch**: Verify all dependencies are built for your system architecture
- **Missing headers**: Check include paths in build script

#### Runtime Issues
- **Data files not found**: Ensure `data/` directory exists and is writable
- **GUI not displaying**: Check wxWidgets installation and compatibility
- **Export functionality**: Verify file permissions and available disk space

### Debug Mode
Enable debug output by adding `-DDEBUG` flag to build script:
```bash
# In build_gui.sh, add to CXXFLAGS
CXXFLAGS="$CXXFLAGS -DDEBUG"
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/new-feature`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature/new-feature`)
5. Create a Pull Request

## 📄 License

This project is developed for educational purposes as part of an Object-Oriented Programming course.

## 👥 Authors

- **ndeshi06** - *Initial development* - [GitHub Profile](https://github.com/ndeshi06)

## 🙏 Acknowledgments

- wxWidgets development team for the excellent GUI framework
- C++ community for best practices and design patterns
- Educational institution for project requirements and guidance

## 📞 Support

For issues, questions, or contributions:
- Create an issue on GitHub
- Check troubleshooting section above
- Review code documentation in source files

---

**Note**: This application is optimized for macOS Apple Silicon. For other platforms, additional configuration may be required.
