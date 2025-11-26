#include "Renderer.h"
#include "Projection.h"
#include <cmath>
#include <algorithm>
#include <cstdint>

// ======================
// Constructor
// ======================
Renderer::Renderer(sf::RenderWindow& win)
: window(win)
{
    screenCenter = sf::Vector2f(
        window.getSize().x * 0.5f,
        window.getSize().y * 0.5f
    );
}

// ======================
// Dibujar estrellas
// ======================
void Renderer::drawStars(
    const StarCatalog& catalog,
    const Vec3& center,
    double scale
) {
    // ✅ SFML 3: PrimitiveType
    sf::VertexArray points(sf::PrimitiveType::Points);

    for (const auto& s : catalog.getStars()) {

        // -----------------------------
        // Proyección 3D → 2D
        // -----------------------------
        sf::Vector2f p = Projection::stereographic(
            s.v,
            center,
            scale,
            screenCenter
        );

        // Detrás del observador
        if (p.x < -1e5f)
            continue;

        // -----------------------------
        // Culling pantalla
        // -----------------------------
        if (p.x < 0 || p.x > static_cast<float>(window.getSize().x) ||
            p.y < 0 || p.y > static_cast<float>(window.getSize().y))
            continue;

        // -----------------------------
        // Brillo por magnitud
        // -----------------------------
        float intensity = std::pow(10.f, -0.4f * static_cast<float>(s.mag));
        intensity = std::clamp(intensity, 0.2f, 1.0f);

        // ✅ SFML 3: std::uint8_t
        sf::Color color(
            static_cast<std::uint8_t>(255 * intensity),
            static_cast<std::uint8_t>(255 * intensity),
            static_cast<std::uint8_t>(255 * intensity)
        );

        // -----------------------------
        // Agregar punto
        // -----------------------------
        sf::Vertex v;
        v.position = p;
        v.color = color;
        
        points.append(v);

    }

    window.draw(points);
}
