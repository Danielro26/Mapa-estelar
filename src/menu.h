#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

struct MenuItem {
    std::string text;
    sf::RectangleShape box;
    sf::Text label;

    sf::Color baseColor;
    sf::Color hoverColor;

    float scale = 1.f;
    bool hovered = false;

    MenuItem(const std::string& t, const sf::Font& font)
        : text(t), label(font) {}
};

class Menu {
public:
    Menu(const sf::Font& font, float startX, float startY);

    void update(sf::Vector2f mousePos);
    void draw(sf::RenderTarget& target);
    void handleClick(sf::Vector2f mousePos, sf::Text& infoText);

private:
    std::vector<MenuItem> items;

    MenuItem createMenuItem(
        const std::string& text,
        float x,
        float y,
        const sf::Font& font,
        sf::Color base
    );
};
