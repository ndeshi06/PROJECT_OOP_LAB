# Badminton Court Management System

A comprehensive Object-Oriented Programming (OOP) application for managing badminton court bookings with GUI interface.

## Features

### Core Functionality
- **User Management**: Registration, login (admin/customer roles)
- **Court Management**: Add/edit/delete courts (admin only)
- **Booking System**: Choose court, date, time slots with availability checking
- **Booking Modification**: Cancel and modify bookings
- **Notifications**: Email reminders (optional)
- **Statistics**: Booking count, history, revenue by day/week/month

### Design Patterns
- **Singleton Pattern**: Manages all booking operations, cancellations, and availability checks
- **Observer Pattern**: Automatic notifications when booking changes occur
- Additional patterns for flexibility and extensibility

### GUI Components
- **Login Window**: Email/password input with role selection
- **Main Window**: Navigation menu for court management, booking, statistics
- **Court Management Widget**: List view with add/edit/delete functionality (admin)
- **Booking Widget**: Court selection, date/time picker, booking cart (customer)
- **Notification Panel**: Upcoming reminders and alerts
- **Statistics Widget**: Revenue charts and booking analytics

## Technical Architecture

### Modules
1. **Authentication & User Management**
   - Registration and login system
   - Role-based access control (admin/customer)

2. **Court Management**
   - CRUD operations for courts
   - Court listing and details

3. **Booking System**
   - Availability calendar view
   - Booking creation and cancellation
   - Conflict resolution

4. **Notification System**
   - Automated email notifications
   - Booking reminders

5. **Statistics & Reporting**
   - Analytics dashboard
   - Revenue and usage reports
   - Interactive charts

6. **GUI Framework**
   - wxWidgets-based interface
   - Signal/slot connections with controllers

## Requirements
- C++17 or later
- wxWidgets library
- CMake 3.16 or later

## Build Instructions
```bash
mkdir build
cd build
cmake ..
make
```

## Usage
1. Run the application
2. Login as admin or register as customer
3. Navigate through the menu system
4. Manage courts (admin) or make bookings (customer)
5. View statistics and reports
