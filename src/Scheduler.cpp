#include "Scheduler.h"
#include <fstream>      // For std::ifstream, std::ofstream
#include <iostream>     // For std::cout, std::cerr, std::endl
#include <iomanip>      // For std::setw, std::setprecision, etc.
#include <algorithm>    // For std::sort, std::max, std::min
#include <cstring>      // For strcmp

// ANSI Color codes
const std::string RESET = "\033[0m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";
const std::string BOLD = "\033[1m";

// Constructor
Scheduler::Scheduler() = default; // Or specific implementation if needed

/**
 * @brief Loads dental operations from the specified file.
 */
bool Scheduler::loadOperations(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << RED << BOLD << "Error: " << RESET << "Could not open operations file: " << filename << std::endl;
        return false;
    }
    char op_name[50];
    float op_price;
    int op_duration;
    while (inFile >> op_name >> op_price >> op_duration) { // Format: <name> <price> <duration> [cite: 5]
        operations.emplace_back(op_name, op_price, op_duration);
    }
    inFile.close();
    if (operations.empty() && filename == "op_details.txt") { // Check only for the main operations file
         std::cerr << YELLOW << BOLD << "Warning: " << RESET << "No operations loaded from " << filename << ". Please check the file format and content." << std::endl;
    }
    return true;
}

/**
 * @brief Finds an operation by name.
 */
const Operation* Scheduler::findOperationByName(const char* opName) const {
    for (const auto& op : operations) {
        if (strcmp(op.name, opName) == 0) { // strcmp for C-style strings [cite: 11]
            return &op;
        }
    }
    return nullptr;
}

/**
 * @brief Loads appointments from the specified file.
 */
bool Scheduler::loadAppointments(const std::string& filename) {
    if (operations.empty() && filename == "app_details.txt") {
        std::cerr << YELLOW << BOLD << "Warning: " << RESET << "Operations should be loaded before appointments for proper functionality." << std::endl;
    }

    std::ifstream inFile(filename);
    if (!inFile) {
        // std::cout << "Info: Appointments file " << filename << " was not found or is empty." << std::endl;
        return true; // It's not an error if the file doesn't exist
    }

    int d, m, y, h, min;
    char opNameBuffer[50];
    // Format: <day> <month> <year> <hour> <minute> <operation_name> [cite: 5]
    while (inFile >> d >> m >> y >> h >> min >> opNameBuffer) {
        const Operation* opPtr = findOperationByName(opNameBuffer);
        if (opPtr) {
            appointments.emplace_back(d, m, y, h, min, *opPtr);
        } else {
            std::cerr << YELLOW << BOLD << "Warning: " << RESET << "Operation '" << opNameBuffer << "' from an appointment was not found. Appointment skipped." << std::endl;
        }
    }
    inFile.close();
    std::sort(this->appointments.begin(), this->appointments.end(), [](const Appointment& a, const Appointment& b){
        if (a.year != b.year) return a.year < b.year;
        if (a.month != b.month) return a.month < b.month;
        if (a.day != b.day) return a.day < b.day;
        if (a.hour != b.hour) return a.hour < b.hour;
        return a.minute < b.minute;
    });
    return true;
}

/**
 * @brief Saves all current appointments to the specified file.
 */
bool Scheduler::saveAppointments(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        std::cerr << RED << BOLD << "Error: " << RESET << "Could not open appointments file for saving: " << filename << std::endl;
        return false;
    }
    for (const auto& app : appointments) {
        // Format: <day> <month> <year> <hour> <minute> <operation_name> [cite: 5]
        outFile << app.day << " " << app.month << " " << app.year << " "
                << app.hour << " " << app.minute << " " << app.operation.name << std::endl;
    }
    outFile.close();
    if (outFile.fail()) {
         std::cerr << RED << BOLD << "Error: " << RESET << "Could not write all data to appointments file: " << filename << std::endl;
        return false;
    }
    return true;
}

/**
 * @brief Displays the list of available dental operations.
 */
void Scheduler::displayOperations() const {
    if (operations.empty()) {
        std::cout << YELLOW << BOLD << "No dental operations available." << RESET << std::endl;
        return;
    }
    std::cout << CYAN << BOLD << "\nAvailable Dental Operations:" << RESET << std::endl;
    std::cout << BLUE << "-----------------------------------------------------------------" << RESET << std::endl;
    std::cout << std::left
              << MAGENTA << BOLD << std::setw(5) << "No."
              << std::setw(25) << "Operation Name"
              << std::setw(15) << "Price (RON)"
              << std::setw(15) << "Duration (min)" << RESET << std::endl;
    std::cout << BLUE << "-----------------------------------------------------------------" << RESET << std::endl;
    int index = 1;
    for (const auto& op : operations) {
        std::cout << std::left
                  << WHITE << std::setw(5) << index++
                  << GREEN << std::setw(25) << op.name
                  << YELLOW << std::fixed << std::setprecision(2) << std::setw(15) << op.price
                  << CYAN << std::setw(15) << op.duration << RESET << std::endl;
    }
    std::cout << BLUE << "-----------------------------------------------------------------" << RESET << "\n" << std::endl;
}

/**
 * @brief Checks if a time slot is available for an operation.
 */
bool Scheduler::isSlotAvailable(int day, int month, int year, int hour, int minute, int operationDuration) const {
    int newAppStartMinutes = hour * 60 + minute;
    int newAppEndMinutes = newAppStartMinutes + operationDuration;

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || newAppEndMinutes > 24 * 60) {
        return false; 
    }

    for (const auto& existingApp : appointments) {
        if (existingApp.day == day && existingApp.month == month && existingApp.year == year) {
            int existingAppStartMinutes = existingApp.hour * 60 + existingApp.minute;
            int existingAppEndMinutes = existingAppStartMinutes + existingApp.operation.duration;
            if (std::max(newAppStartMinutes, existingAppStartMinutes) < std::min(newAppEndMinutes, existingAppEndMinutes)) {
                return false; 
            }
        }
    }
    return true; 
}

/**
 * @brief Checks if a specific minute is covered by any appointment.
 */
bool Scheduler::isExactMinuteAvailable(int day, int month, int year, int hour, int minute) const {
    int checkTimeInMinutes = hour * 60 + minute;
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) return false;

    for (const auto& app : appointments) {
        if (app.day == day && app.month == month && app.year == year) {
            int appStartMinutes = app.hour * 60 + app.minute;
            int appEndMinutes = appStartMinutes + app.operation.duration;
            if (checkTimeInMinutes >= appStartMinutes && checkTimeInMinutes < appEndMinutes) {
                return false; 
            }
        }
    }
    return true;
}

/**
 * @brief Schedules a new appointment.
 */
bool Scheduler::scheduleAppointment(int day, int month, int year, int hour, int minute, int operation_number) {
    if (operation_number <= 0 || static_cast<size_t>(operation_number) > operations.size()) {
        std::cerr << RED << BOLD << "Error: " << RESET << "Invalid operation number." << std::endl;
        displayOperations(); 
        return false;
    }

    const Operation& selectedOp = operations[operation_number - 1]; 

    if (month < 1 || month > 12 || day < 1 || day > 31 || year < 2024 || year > 2050 ||
        hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        std::cerr << RED << BOLD << "Error: " << RESET << "Invalid date or time for appointment." << std::endl;
        return false;
    }
    
    int proposedEndTimeMinutes = hour * 60 + minute + selectedOp.duration;
    if (proposedEndTimeMinutes > 24*60) {
        std::cout << RED << BOLD << "Appointment for '" << selectedOp.name << "' on "
                  << day << "/" << month << "/" << year << " at "
                  << std::setfill('0') << std::setw(2) << hour << ":"
                  << std::setfill('0') << std::setw(2) << minute
                  << " cannot be scheduled (extends past midnight)." << RESET << std::endl;
        std::cout << std::setfill(' '); 
        return false;
    }

    if (!isSlotAvailable(day, month, year, hour, minute, selectedOp.duration)) {
        std::cout << RED << BOLD << "Time slot " << day << "/" << month << "/" << year
                  << " at " << std::setfill('0') << std::setw(2) << hour << ":"
                  << std::setfill('0') << std::setw(2) << minute
                  << " for '" << selectedOp.name << "' (duration " << selectedOp.duration
                  << " min) is not available." << RESET << std::endl;
        std::cout << std::setfill(' '); 
        return false;
    }

    appointments.emplace_back(day, month, year, hour, minute, selectedOp);
    std::cout << GREEN << BOLD << "Appointment scheduled: '" << selectedOp.name
              << "' on " << day << "/" << month << "/" << year
              << " at " << std::setfill('0') << std::setw(2) << hour << ":"
              << std::setfill('0') << std::setw(2) << minute << "." << RESET << std::endl;
    std::cout << std::setfill(' '); 

    std::sort(this->appointments.begin(), this->appointments.end(), [](const Appointment& a, const Appointment& b){
        if (a.year != b.year) return a.year < b.year;
        if (a.month != b.month) return a.month < b.month;
        if (a.day != b.day) return a.day < b.day;
        if (a.hour != b.hour) return a.hour < b.hour;
        return a.minute < b.minute;
    });

    return true;
}

/**
 * @brief Displays the history of all appointments.
 */
void Scheduler::displayAppointmentHistory() const {
    if (appointments.empty()) {
        std::cout << YELLOW << BOLD << "No appointments in history." << RESET << std::endl;
        return;
    }
    std::cout << CYAN << BOLD << "\nAppointment History:" << RESET << std::endl;
    std::cout << BLUE << "------------------------------------------------------------------------------------" << RESET << std::endl;
    std::cout << std::left
              << MAGENTA << BOLD << std::setw(12) << "Date"
              << std::setw(8) << "Time"
              << std::setw(25) << "Operation"
              << std::setw(15) << "Price (RON)"
              << std::setw(15) << "Duration (min)" << RESET << std::endl;
    std::cout << BLUE << "------------------------------------------------------------------------------------" << RESET << std::endl;

    for (const auto& app : appointments) {
        std::cout << std::left
                  << WHITE << std::setfill('0') << std::setw(2) << app.day << "/"
                  << std::setfill('0') << std::setw(2) << app.month << "/"
                  << std::setw(4) << app.year << std::setfill(' ') << "  " 
                  << CYAN << std::setfill('0') << std::setw(2) << app.hour << ":"
                  << std::setfill('0') << std::setw(2) << app.minute << std::setfill(' ') << "   " 
                  << GREEN << std::setw(25) << app.operation.name
                  << YELLOW << std::fixed << std::setprecision(2) << std::setw(15) << app.operation.price
                  << MAGENTA << std::setw(15) << app.operation.duration << RESET << std::endl;
    }
    std::cout << BLUE << "------------------------------------------------------------------------------------" << RESET << "\n" << std::endl;
    std::cout << std::setfill(' ');
}