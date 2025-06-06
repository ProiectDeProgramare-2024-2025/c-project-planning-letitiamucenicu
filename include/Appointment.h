#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include "Operation.h" // Appointment conține un obiect Operation [cite: 3]

class Appointment {
public:
    int day, month, year, hour, minute; // Data și ora programării [cite: 3]
    Operation operation;                // Operațiunea asociată [cite: 3]

    // Constructor implicit
    Appointment() : day(0), month(0), year(0), hour(0), minute(0) {}

    // Constructor parametrizat
    Appointment(int d, int m, int y, int h, int min, const Operation& op)
        : day(d), month(m), year(y), hour(h), minute(min), operation(op) {}
};

#endif // APPOINTMENT_H