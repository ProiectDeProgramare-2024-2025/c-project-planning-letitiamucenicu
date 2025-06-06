#ifndef OPERATION_H
#define OPERATION_H

#include <cstring> // Pentru strncpy

class Operation {
public:
    char name[50];    // Numele operațiunii [cite: 2, 7]
    float price;      // Prețul operațiunii [cite: 2]
    int duration;     // Durata în minute [cite: 2]

    // Constructor implicit
    Operation() : price(0.0f), duration(0) {
        name[0] = '\0'; // Inițializare nume gol
    }

    // Constructor parametrizat
    Operation(const char* n, float p, int d) : price(p), duration(d) {
        strncpy(name, n, 49); // Copiază numele
        name[49] = '\0';      // Asigură terminarea cu null
    }
};

#endif // OPERATION_H