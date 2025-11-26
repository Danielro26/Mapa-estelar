#pragma once
#include <string>

struct Vec3 {
    double x, y, z;
};

class Star {
public:
    Vec3 v;             // Vector unitario de posición en 3D
    int hip;            // 🔧 identificador HIP
    double mag;         // Magnitud aparente
    double bv;          // Índice de color B-V (si está disponible)
    double temperature; // Temperatura en Kelvin (opcional)
    double ra;          // Ascensión recta (grados)
    double dec;         // Declination (grados)
    std::string name;   // Nombre o identificador (si existe)

    Star() : v{0,0,0}, mag(0), bv(0), temperature(0), ra(0), dec(0), name("") {}
};
