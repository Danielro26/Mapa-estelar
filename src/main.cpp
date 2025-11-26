#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include "StarCatalog.h"

constexpr double PI = 3.14159265358979323846;

// ======================
// Utilidades matemáticas
// ======================
double dot(const Vec3& a, const Vec3& b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 cross(const Vec3& a, const Vec3& b) { return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x}; }
Vec3 normalize(const Vec3& v) { double mag = std::sqrt(dot(v,v)); return {v.x/mag, v.y/mag, v.z/mag}; }
Vec3 rotateRodrigues(const Vec3& v, const Vec3& k, double theta) {
    double cosT = std::cos(theta), sinT = std::sin(theta);
    Vec3 term1 = {v.x*cosT, v.y*cosT, v.z*cosT};
    Vec3 kxv = cross(k,v);
    Vec3 term2 = {kxv.x*sinT, kxv.y*sinT, kxv.z*sinT};
    double kdotv = dot(k,v);
    Vec3 term3 = {k.x*kdotv*(1-cosT), k.y*kdotv*(1-cosT), k.z*kdotv*(1-cosT)};
    return {term1.x+term2.x+term3.x, term1.y+term2.y+term3.y, term1.z+term2.z+term3.z};
}

// Tamaño según magnitud
float sizeFromMagnitude(double mag) {
    return std::max(0.4f, static_cast<float>(3.0 * std::pow(10.0, -0.2 * mag)));
}

// Colores según magnitud
sf::Color colorFromMagnitude(double mag) {
    if (mag <= 2.0) return sf::Color::White;
    else if (mag <= 4.0) return sf::Color(200,200,255);
    else if (mag <= 6.0) return sf::Color(255,255,200);
    else return sf::Color(255,180,180);
}

// Colores según temperatura
sf::Color colorFromTemperature(double temp) {
    temp = std::clamp(temp, 2500.0, 40000.0);
    double t = (temp - 2500)/(40000-2500);
    if (t>=0.8) return sf::Color(200,220,255);   // azul
    else if (t>=0.6) return sf::Color::White;    // blanco
    else if (t>=0.4) return sf::Color(255,255,200); // amarillo
    else if (t>=0.2) return sf::Color(255,200,150); // naranja
    else return sf::Color(255,150,150);          // rojo
}

// Colores según B-V
sf::Color colorFromBV(double bv) {
    if (bv<0.0) return sf::Color(200,220,255);
    else if (bv<0.5) return sf::Color::White;
    else if (bv<1.0) return sf::Color(255,255,200);
    else if (bv<1.5) return sf::Color(255,200,150);
    else return sf::Color(255,150,150);
}

// ======================
// Programa principal
// ======================
int main() {
    sf::RenderWindow window(sf::VideoMode({1200,800}), "Mapa Estelar - Hipparcos");
    window.setFramerateLimit(60);

    StarCatalog catalog;
    if (!catalog.loadFromCSV("data/hipparcos_full.csv")) {
        std::cerr << "Error cargando hipparcos_full.csv\n";
        return -1;
    }

    double centerRA = 0.0, centerDEC = 0.0;
    double zoom = 300.0; // 🔧 Zoom inicial más bajo
    double magLimit = 9.0;

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (const auto* e = event->getIf<sf::Event::MouseWheelScrolled>()) {
                zoom *= (e->delta > 0) ? 1.15 : 0.85;
            }
            if (const auto* e = event->getIf<sf::Event::KeyPressed>()) {
                double step = 5.0;
                switch (e->code) {
                    // Flechas
                    case sf::Keyboard::Key::Left:  centerRA  -= step; break;
                    case sf::Keyboard::Key::Right: centerRA  += step; break;
                    case sf::Keyboard::Key::Up:    centerDEC += step; break;
                    case sf::Keyboard::Key::Down:  centerDEC -= step; break;

                    // WASD
                    case sf::Keyboard::Key::A:     centerRA  -= step; break;
                    case sf::Keyboard::Key::D:     centerRA  += step; break;
                    case sf::Keyboard::Key::W:     centerDEC += step; break;
                    case sf::Keyboard::Key::S:     centerDEC -= step; break;

                    default: break;
                }
            }
        }

        window.clear(sf::Color::Black);

        // Vector centro
        double raRad  = centerRA * PI / 180.0;
        double decRad = centerDEC * PI / 180.0;
        Vec3 centerVec = { std::cos(decRad)*std::cos(raRad),
                           std::cos(decRad)*std::sin(raRad),
                           std::sin(decRad) };

        Vec3 north = {0.0,0.0,1.0};
        Vec3 axis = cross(centerVec,north);
        double axisLen = std::sqrt(dot(axis,axis));
        bool useRotation = axisLen>1e-6;
        Vec3 axisNorm = useRotation ? normalize(axis) : Vec3{0,0,1};
        double angle = useRotation ? std::acos(dot(centerVec,north)) : 0.0;

        auto winSize = window.getSize();
        float winW = static_cast<float>(winSize.x);
        float winH = static_cast<float>(winSize.y);

        // Dibujar estrellas
        for (const Star& s : catalog.getStars()) {
            if (s.mag > magLimit) continue;

            Vec3 rv = useRotation ? rotateRodrigues(s.v, axisNorm, angle) : s.v;
            if (rv.z <= 0.0) continue;

            double X = rv.x;
            double Y = rv.y;

            float screenX = winW * 0.5f + static_cast<float>(X * zoom);
            float screenY = winH * 0.5f - static_cast<float>(Y * zoom);
            float size = sizeFromMagnitude(s.mag) * static_cast<float>(zoom / 300.0);

            if (screenX < -50 || screenX > winW + 50 || screenY < -50 || screenY > winH + 50) continue;

            sf::CircleShape star(size);

            // 🔧 Color avanzado
            if (s.temperature > 0.0)
                star.setFillColor(colorFromTemperature(s.temperature));
            else if (s.bv != 0.0)
                star.setFillColor(colorFromBV(s.bv));
            else
                star.setFillColor(colorFromMagnitude(s.mag));

            star.setOrigin({size,size});
            star.setPosition({screenX,screenY});
            window.draw(star);
        }

        window.display();
    }

    return 0;
}