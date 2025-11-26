#ifndef PROJECTION_H
#define PROJECTION_H

#include <SFML/System/Vector2.hpp>
#include "Star.h"

// ======================
// Proyecciones astronómicas
// ======================
class Projection {
public:
    // Proyección estereográfica
    // Entrada:
    //   v -> vector unitario 3D de la estrella
    //   center -> vector unitario 3D del centro de la vista
    //   scale -> factor de zoom / escala
    //   screenCenter -> centro de la pantalla
    //
    // Devuelve:
    //   Coordenadas 2D de pantalla
    static sf::Vector2f stereographic(
        const Vec3& v,
        const Vec3& center,
        double scale,
        const sf::Vector2f& screenCenter
    );

    // (Futuro) Proyección Hammer-Aitoff
    // static sf::Vector2f hammerAitoff(...);
};

#endif // PROJECTION_H
