#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include "Operation.h"    // Scheduler folosește Operation [cite: 8]
#include "Appointment.h"  // Scheduler folosește Appointment [cite: 9]

class Scheduler {
public:
    std::vector<Operation> operations;     // Lista operațiunilor disponibile [cite: 4, 8]
    std::vector<Appointment> appointments; // Lista programărilor efectuate [cite: 4, 9]

    Scheduler(); // Constructor

    // Metode pentru gestionarea datelor
    bool loadOperations(const std::string& filename);
    bool loadAppointments(const std::string& filename);
    bool saveAppointments(const std::string& filename) const;

    // Metode pentru funcționalitatea sistemului
    void displayOperations() const;
    bool isSlotAvailable(int day, int month, int year, int hour, int minute, int operationDuration) const;
    bool isExactMinuteAvailable(int day, int month, int year, int hour, int minute) const;
    bool scheduleAppointment(int day, int month, int year, int hour, int minute, int operation_number);
    void displayAppointmentHistory() const;

private:
    const Operation* findOperationByName(const char* opName) const;
};

#endif // SCHEDULER_H