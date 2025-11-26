#include "StarCatalog.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>

constexpr double PI = 3.14159265358979323846;

StarCatalog::StarCatalog() {}

bool StarCatalog::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << filename << "\n";
        return false;
    }

    std::string line;
    std::getline(file, line); // saltar encabezado

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        Star s;

        // hip (id, lo puedes ignorar si no lo usas)
        std::getline(ss, token, ','); s.hip = std::stoi(token);

        // ra
        std::getline(ss, token, ','); s.ra = std::stod(token);

        // dec
        std::getline(ss, token, ','); s.dec = std::stod(token);

        // mag
        std::getline(ss, token, ','); s.mag = std::stod(token);

        // bv
        std::getline(ss, token, ','); s.bv = std::stod(token);

        // temperature
        std::getline(ss, token, ','); s.temperature = std::stod(token);

        // Convertir RA/DEC a vector unitario 3D
        double raRad  = s.ra * PI / 180.0;
        double decRad = s.dec * PI / 180.0;
        s.v.x = std::cos(decRad) * std::cos(raRad);
        s.v.y = std::cos(decRad) * std::sin(raRad);
        s.v.z = std::sin(decRad);

        stars.push_back(s);
    }

    return true;
}

const std::vector<Star>& StarCatalog::getStars() const {
    return stars;
}