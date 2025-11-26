#include "Projection.h"
#include <cmath>

// ======================
// Utilidades matemáticas
// ======================
static double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

static Vec3 normalize(const Vec3& v) {
    double len = std::sqrt(dot(v, v));
    return { v.x / len, v.y / len, v.z / len };
}

// ==========================================
// Rotación usando fórmula de Rodrigues
// ==========================================
static Vec3 rotateRodrigues(const Vec3& v, const Vec3& k, double theta) {
    double cosT = std::cos(theta);
    double sinT = std::sin(theta);

    Vec3 kxv = cross(k, v);
    double kdotv = dot(k, v);

    return {
        v.x * cosT + kxv.x * sinT + k.x * kdotv * (1 - cosT),
        v.y * cosT + kxv.y * sinT + k.y * kdotv * (1 - cosT),
        v.z * cosT + kxv.z * sinT + k.z * kdotv * (1 - cosT)
    };
}

// ==========================================
// Proyección estereográfica
// ==========================================
sf::Vector2f Projection::stereographic(
    const Vec3& v,
    const Vec3& center,
    double scale,
    const sf::Vector2f& screenCenter
) {
    // Polo norte (dirección de proyección)
    const Vec3 north{ 0.0, 0.0, 1.0 };
    Vec3 rv = v;

    // -----------------------------
    // Rotar esfera para centrar
    // -----------------------------
    Vec3 axis = cross(center, north);
    double axisLen = std::sqrt(dot(axis, axis));

    if (axisLen > 1e-9) {
        axis = normalize(axis);
        double angle = std::acos(dot(center, north));
        rv = rotateRodrigues(v, axis, angle);
    }

    // -----------------------------
    // Culling: estrella detrás
    // -----------------------------
    if (rv.z <= 0.0) {
        return { -1e6f, -1e6f };
    }

    // -----------------------------
    // Proyección estereográfica
    // -----------------------------
    double denom = 1.0 - rv.z;
    if (denom < 1e-6)
        denom = 1e-6;

    double x = rv.x / denom;
    double y = rv.y / denom;

    // -----------------------------
    // Escalar y adaptar a pantalla
    // -----------------------------
    float screenX = static_cast<float>(screenCenter.x + x * scale);
    float screenY = static_cast<float>(screenCenter.y - y * scale);

    return { screenX, screenY };
}
