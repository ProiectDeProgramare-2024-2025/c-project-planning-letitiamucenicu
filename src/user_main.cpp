#include <iostream>
#include <string>
#include <stdexcept> // For std::invalid_argument, std::out_of_range
#include <iomanip>   // For std::setfill, std::setw
#include "Scheduler.h"

// Constants for file names - expected to be in the same directory as the executable
const std::string OPS_FILENAME = "op_details.txt";
const std::string APPS_FILENAME = "app_details.txt";

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

void printUserUsage() {
    std::cerr << CYAN << BOLD << "Usage User App: " << RESET << "./user_app <command> [arguments...]" << std::endl;
    std::cerr << BLUE << BOLD << "Available commands for user:" << RESET << std::endl;
    std::cerr << GREEN << "  view_operations                                          " << RESET << "- Display available dental operations" << std::endl;
    std::cerr << GREEN << "  check_availability <day> <month> <year> <hour> <minute>  " << RESET << "- Check if a time slot is available [cite: 1]" << std::endl;
    std::cerr << GREEN << "  schedule <day> <month> <year> <hour> <minute> <op_nr>    " << RESET << "- Schedule a new appointment [cite: 1]" << std::endl;
    std::cerr << GREEN << "  view_history                                             " << RESET << "- Display appointment history [cite: 1]" << std::endl;
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    if (argc < 2) {
        printUserUsage();
        return 1;
    }

    Scheduler scheduler;

    // User loads operations and appointments at startup
    if (!scheduler.loadOperations(OPS_FILENAME)) {
        std::cerr << RED << BOLD << "Critical error: " << RESET << "Could not load operations from " << OPS_FILENAME << ". The program cannot continue." << std::endl;
        return 1;
    }
    scheduler.loadAppointments(APPS_FILENAME);

    std::string command = argv[1];

    try {
        if (command == "view_operations") {
            if (argc != 2) { printUserUsage(); return 1; }
            scheduler.displayOperations();
        } else if (command == "check_availability") {
            if (argc != 7) { printUserUsage(); return 1; }
            int day = std::stoi(argv[2]);
            int month = std::stoi(argv[3]);
            int year = std::stoi(argv[4]);
            int hour = std::stoi(argv[5]);
            int minute = std::stoi(argv[6]);

            if (scheduler.isExactMinuteAvailable(day, month, year, hour, minute)) {
                std::cout << GREEN << BOLD << "Time slot (time "
                          << std::setfill('0') << std::setw(2) << hour << ":"
                          << std::setfill('0') << std::setw(2) << minute
                          << " on " << day << "/" << month << "/" << year 
                          << ") is AVAILABLE" << RESET << GREEN << " (exact minute is not covered)." << RESET << std::endl;
            } else {
                std::cout << RED << BOLD << "Time slot (time "
                          << std::setfill('0') << std::setw(2) << hour << ":"
                          << std::setfill('0') << std::setw(2) << minute
                          << " on " << day << "/" << month << "/" << year 
                          << ") is NOT AVAILABLE" << RESET << RED << " (exact minute is covered)." << RESET << std::endl;
            }
            std::cout << std::setfill(' '); // Reset fill character
        } else if (command == "schedule") {
            if (argc != 8) { printUserUsage(); return 1; }
            int day = std::stoi(argv[2]);
            int month = std::stoi(argv[3]);
            int year = std::stoi(argv[4]);
            int hour = std::stoi(argv[5]);
            int minute = std::stoi(argv[6]);
            int op_num = std::stoi(argv[7]);

            if (scheduler.scheduleAppointment(day, month, year, hour, minute, op_num)) {
                // According to "app_details.txt: Updated with every new appointment"[cite: 12], we save automatically.
                if (!scheduler.saveAppointments(APPS_FILENAME)) {
                    std::cerr << RED << BOLD << "CRITICAL WARNING: " << RESET << "Appointment was added to memory, but automatic saving to file "
                              << APPS_FILENAME << RED << " FAILED!" << RESET << std::endl;
                    std::cerr << YELLOW << "Please contact the administrator." << RESET << std::endl;
                } else {
                    // std::cout << "Appointment was saved automatically." << std::endl; // Message already clear from scheduleAppointment
                }
            }
        } else if (command == "view_history") {
            if (argc != 2) { printUserUsage(); return 1; }
            scheduler.displayAppointmentHistory();
        } else {
            std::cerr << RED << BOLD << "Error: " << RESET << "Unknown user command '" << command << "'" << std::endl;
            printUserUsage();
            return 1;
        }
    } catch (const std::invalid_argument& ia) {
        std::cerr << RED << BOLD << "Error: " << RESET << "Invalid numeric argument. Make sure numeric parameters are correct." << std::endl;
        std::cerr << YELLOW << "Details: " << RESET << ia.what() << std::endl;
        printUserUsage();
        return 1;
    } catch (const std::out_of_range& oor) {
        std::cerr << RED << BOLD << "Error: " << RESET << "Numeric argument out of allowed range." << std::endl;
        std::cerr << YELLOW << "Details: " << RESET << oor.what() << std::endl;
        printUserUsage();
        return 1;
    } catch (const std::exception& e) {
        std::cerr << RED << BOLD << "An unexpected error occurred: " << RESET << e.what() << std::endl;
        return 1;
    }
    
    std::cout << std::setfill(' '); // Reset fill character
    return 0;
}