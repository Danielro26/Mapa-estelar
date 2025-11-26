#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "StarCatalog.h"
#include "Projection.h"

// ======================
// Motor de renderizado
// ======================
class Renderer {
public:
    // Constructor
    Renderer(sf::RenderWindow& window);

    // Dibuja todas las estrellas del catálogo
    void drawStars(
        const StarCatalog& catalog,
        const Vec3& center,
        double scale
    );

private:
    sf::RenderWindow& window;
    sf::Vector2f screenCenter;
};

#endif // RENDERER_H
