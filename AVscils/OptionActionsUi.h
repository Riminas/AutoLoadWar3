#pragma once
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include "StringConvector.h"
#include "FontSelector.h"

// Структура для хранения данных
class OptionActionUi {
private:
    sf::Sprite sprite;
    sf::Text text; // Основной текст
public:
    // Конструктор для инициализации данных
    void initialize(const std::wstring& str, const sf::Texture& texture)
    {
        sprite.setTexture(texture);
        sprite.setTextureRect(sf::Rect{ 0, 544, 512, 32 });

        // Инициализация основного текста
        FontSelector().getFontForText(text);
        if (str.empty()) {
            text.setString("Путь до папки с сохранениями пуст");
            text.setFillColor(sf::Color(202, 51, 62));
        }
        else {
            text.setString(str);
            text.setFillColor(sf::Color::White);
        }
        text.setCharacterSize(14);
    }

    // Метод для установки позиции
    inline void setPosition(const float x, const float y) {
        StringConvector().adjustTextToFit(text, 380); // 236 = 512 / 2 - 20
        text.setPosition(20 + x, 4 + y);
        sprite.setPosition(x, y);
    }

    inline void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        G_WINDOW.draw(text);
    }

    inline bool isClicked(const sf::Vector2f& mouseButton) const { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }

    inline bool isClickedButton(const sf::Vector2f& mouseButton) {
        const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        return mouseButton2.x >= 445 && mouseButton2.x < 490;
    }
};