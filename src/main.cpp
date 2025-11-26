#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

#include "StarCatalog.h"
#include "Menu.h"

constexpr double PI = 3.14159265358979323846;

/* =======================
   Matemática 3D
======================= */
double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Vec3 normalize(const Vec3& v) {
    double mag = std::sqrt(dot(v, v));
    if (mag < 1e-9) return {0, 0, 0};
    return { v.x / mag, v.y / mag, v.z / mag };
}

Vec3 rotateRodrigues(const Vec3& v, const Vec3& k, double theta) {
    double cosT = std::cos(theta);
    double sinT = std::sin(theta);

    Vec3 term1 = { v.x * cosT, v.y * cosT, v.z * cosT };
    Vec3 kxv   = cross(k, v);
    Vec3 term2 = { kxv.x * sinT, kxv.y * sinT, kxv.z * sinT };

    double kdotv = dot(k, v);
    Vec3 term3 = {
        k.x * kdotv * (1 - cosT),
        k.y * kdotv * (1 - cosT),
        k.z * kdotv * (1 - cosT)
    };

    return {
        term1.x + term2.x + term3.x,
        term1.y + term2.y + term3.y,
        term1.z + term2.z + term3.z
    };
}

/* =======================
   Apariencia estrellas
======================= */
float sizeFromMagnitude(double mag) {
    return std::max(
        0.4f,
        static_cast<float>(3.0 * std::pow(10.0, -0.2 * mag))
    );
}

sf::Color colorFromMagnitude(double mag) {
    if (mag <= 2.0)      return sf::Color::White;
    else if (mag <= 4.0) return sf::Color(200, 200, 255);
    else if (mag <= 6.0) return sf::Color(255, 255, 200);
    else                return sf::Color(255, 180, 180);
}

/* =======================
   MAIN — SFML 3
======================= */
int main() {

    sf::RenderWindow window(
        sf::VideoMode({1200u, 800u}),
        "Mapa Estelar - Hipparcos (SFML 3)"
    );

    window.setFramerateLimit(60);

    /* ---------- Fuente ---------- */
    sf::Font font;
    if (!font.openFromFile("data/NewRocker-Regular.ttf")) {
        std::cerr << "Error: no se pudo cargar la fuente\n";
        return -1;
    }

    /* ---------- UI ---------- */
    Menu menu(font, 20.f, 20.f);

    sf::Text infoText(font);
    infoText.setCharacterSize(18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({20.f, 400.f});
    infoText.setString("");

    /* ---------- Catálogo ---------- */
    StarCatalog catalog;
    if (!catalog.loadFromCSV("data/hipparcos_full.csv")) {
        std::cerr << "Error: no se pudo cargar el catálogo\n";
        return -1;
    }

    /* ---------- Cámara ---------- */
    double centerRA  = 0.0;
    double centerDEC = 0.0;
    double zoom      = 300.0;
    double magLimit  = 9.0;

    /* =======================
       Loop principal
    ======================= */
    while (window.isOpen()) {

        while (auto event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                zoom *= (wheel->delta > 0.f) ? 1.15 : 0.85;
                zoom = std::clamp(zoom, 50.0, 2000.0);
            }

            if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    menu.handleClick(
                        {
                            static_cast<float>(click->position.x),
                            static_cast<float>(click->position.y)
                        },
                        infoText
                    );
                }
            }
        }

        /* ---------- Movimiento continuo ---------- */
        constexpr double step = 0.5;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) centerRA  -= step;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) centerRA  += step;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) centerDEC += step;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) centerDEC -= step;

        /* ---------- Render ---------- */
        window.clear(sf::Color::Black);

        double raRad  = centerRA  * PI / 180.0;
        double decRad = centerDEC * PI / 180.0;

        Vec3 centerVec {
            std::cos(decRad) * std::cos(raRad),
            std::cos(decRad) * std::sin(raRad),
            std::sin(decRad)
        };

        Vec3 north{0, 0, 1};
        Vec3 axis = cross(centerVec, north);

        double axisLen = std::sqrt(dot(axis, axis));
        bool rotate = axisLen > 1e-6;

        Vec3 axisNorm = rotate ? normalize(axis) : Vec3{0, 0, 1};
        double angle  = rotate ? std::acos(dot(centerVec, north)) : 0.0;

        auto size = window.getSize();
        float w = static_cast<float>(size.x);
        float h = static_cast<float>(size.y);

        for (const Star& s : catalog.getStars()) {

            if (s.mag > magLimit) continue;

            Vec3 rv = rotate
                ? rotateRodrigues(s.v, axisNorm, angle)
                : s.v;

            if (rv.z <= 0.0) continue;

            float x = w * 0.5f + static_cast<float>(rv.x * zoom);
            float y = h * 0.5f - static_cast<float>(rv.y * zoom);

            float r = sizeFromMagnitude(s.mag)
                    * static_cast<float>(zoom / 300.0);

            sf::CircleShape star(r);
            star.setOrigin({r, r});
            star.setPosition({x, y});
            star.setFillColor(colorFromMagnitude(s.mag));

            window.draw(star);
        }

        menu.draw(window);
        window.draw(infoText);

        window.display();
    }

    return 0;
}