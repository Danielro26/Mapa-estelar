#include "Menu.h"
#include <cstdint>

/* ==========================
   Ayuda: interpolación
========================== */
static sf::Color lerp(const sf::Color& a, const sf::Color& b, float t) {
    return sf::Color(
        static_cast<std::uint8_t>(a.r + (b.r - a.r) * t),
        static_cast<std::uint8_t>(a.g + (b.g - a.g) * t),
        static_cast<std::uint8_t>(a.b + (b.b - a.b) * t)
    );
}


/* ==========================
   Constructor
========================== */
Menu::Menu(const sf::Font& font, float startX, float startY) {

    items.push_back(createMenuItem(
        "Magnitud estelar", startX, startY,
        font, sf::Color(40, 80, 160)));

    items.push_back(createMenuItem(
        "Temperatura", startX, startY + 70,
        font, sf::Color(180, 100, 40)));

    items.push_back(createMenuItem(
        "Índice B-V", startX, startY + 140,
        font, sf::Color(70, 150, 80)));
}

/* ==========================
   Crear botón
========================== */
MenuItem Menu::createMenuItem(
    const std::string& text,
    float x, float y,
    const sf::Font& font,
    sf::Color base
) {
    MenuItem item(text, font);

    item.baseColor = base;
    item.hoverColor = lerp(base, sf::Color::White, 0.25f);

    item.box.setSize({280.f, 48.f});
    item.box.setOrigin({140.f, 24.f});
    item.box.setPosition({x + 140.f, y + 24.f});
    item.box.setFillColor(base);

    item.label.setString(text);
    item.label.setCharacterSize(18);
    item.label.setFillColor(sf::Color::White);
    item.label.setPosition({x + 20.f, y + 12.f});

    return item;
}

/* ==========================
   Animaciones (HOVER)
========================== */
void Menu::update(sf::Vector2f mousePos) {

    for (auto& item : items) {

        bool nowHovered = item.box.getGlobalBounds().contains(mousePos);
        item.hovered = nowHovered;

        /* Suavizado */
        float targetScale = nowHovered ? 1.05f : 1.f;
        item.scale += (targetScale - item.scale) * 0.15f;

        item.box.setScale({item.scale, item.scale});

        /* Color suave */
        item.box.setFillColor(
            nowHovered ? item.hoverColor : item.baseColor
        );
    }
}

/* ==========================
   Dibujar
========================== */
void Menu::draw(sf::RenderTarget& target) {
    for (auto& item : items) {
        target.draw(item.box);
        target.draw(item.label);
    }
}

/* ==========================
   Click
========================== */
void Menu::handleClick(sf::Vector2f mousePos, sf::Text& infoText) {

    for (auto& item : items) {
        if (item.box.getGlobalBounds().contains(mousePos)) {

            if (item.text == "Magnitud estelar") {
                infoText.setString(
                    "Magnitud estelar:\n"
                    "Mide el brillo aparente de una estrella.\n"
                    "Valores bajos = más brillo\n"
                    "Valores altos = menos brillo"
                );
            }
            else if (item.text == "Temperatura") {
                infoText.setString(
                    "Temperatura superficial:\n"
                    "Estrellas calientes: azul/blanco\n"
                    "Estrellas frías: amarillas/rojas"
                );
            }
            else if (item.text == "Índice B-V") {
                infoText.setString(
                    "Índice B-V:\n"
                    "Relaciona color con temperatura.\n"
                    "B-V bajo: azul\n"
                    "B-V alto: rojo"
                );
            }
        }
    }
}
