#ifndef STARCATALOG_H
#define STARCATALOG_H

#include <vector>
#include <string>
#include "Star.h"

class StarCatalog {
public:
    // Constructor
    StarCatalog();

    // Cargar catálogo desde CSV
    bool loadFromCSV(const std::string& filename);

    // Acceso de solo lectura al catálogo
    const std::vector<Star>& getStars() const;

private:
    std::vector<Star> stars;
};

#endif // STARCATALOG_H