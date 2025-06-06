#include <iostream>
#include <string>
#include <stdexcept> // For std::invalid_argument, std::out_of_range
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

void printAdminUsage() {
    std::cerr << CYAN << BOLD << "Usage Admin App: " << RESET << "./admin_app <command> [arguments...]" << std::endl;
    std::cerr << BLUE << BOLD << "Available commands for admin:" << RESET << std::endl;
    std::cerr << GREEN << "  view_operations          " << RESET << "- Display available dental operations" << std::endl;
    std::cerr << GREEN << "  view_history             " << RESET << "- Display history of all appointments" << std::endl;
    std::cerr << GREEN << "  save_and_exit            " << RESET << "- Save all appointments and close the program" << std::endl;
    // "Adding available dental operations" [cite: 1] is done by manually editing op_details.txt
    // "Loading and displaying scheduled appointments" [cite: 1] is covered by view_history
    // "Saving the appointment history" [cite: 1] is covered by save_and_exit
}

int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    if (argc < 2) {
        printAdminUsage();
        return 1;
    }

    Scheduler scheduler;

    // Admin loads operations and appointments at startup
    if (!scheduler.loadOperations(OPS_FILENAME)) {
        std::cerr << RED << BOLD << "Critical error: " << RESET << "Could not load operations from " << OPS_FILENAME << ". Please check the file." << std::endl;
        return 1;
    }
    scheduler.loadAppointments(APPS_FILENAME); // It's OK if app_details.txt doesn't exist initially

    std::string command = argv[1];

    try {
        if (command == "view_operations") {
            if (argc != 2) { printAdminUsage(); return 1; }
            scheduler.displayOperations();
        } else if (command == "view_history") {
            if (argc != 2) { printAdminUsage(); return 1; }
            scheduler.displayAppointmentHistory();
        } else if (command == "save_and_exit") {
            if (argc != 2) { printAdminUsage(); return 1; }
            if (scheduler.saveAppointments(APPS_FILENAME)) {
                std::cout << GREEN << BOLD << "Appointments successfully saved to " << RESET << APPS_FILENAME << GREEN << ". Admin app is closing." << RESET << std::endl;
            } else {
                std::cout << RED << BOLD << "Failed to save appointments to " << RESET << APPS_FILENAME << RED << ". Admin app is closing anyway." << RESET << std::endl;
                return 2; // Error code for save failure
            }
            return 0; // Normal exit after save_and_exit
        } else {
            std::cerr << RED << BOLD << "Error: " << RESET << "Unknown admin command '" << command << "'" << std::endl;
            printAdminUsage();
            return 1;
        }
    } catch (const std::exception& e) { // Catch more general exceptions
        std::cerr << RED << BOLD << "An error occurred: " << RESET << e.what() << std::endl;
        return 1;
    }

    return 0; // Normal exit for commands that are not 'save_and_exit'
}