#pragma once
#include <Windows.h>
#include <SFML/Graphics.hpp>
#include "StringConvector.h"
#include "Global.h"

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
        setString(str);
        // Инициализация основного текста
        text.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        text.setCharacterSize(13);
    }

    // Метод для установки позиции
    inline void setPosition(const float x, const float y) {
        StringConvector().adjustTextToFit(text, 380); // 236 = 512 / 2 - 20
        text.setPosition(32 + x, 7 + y);
        sprite.setPosition(x, y);
    }
    
    inline void setString(const std::wstring& str) {
        if (str.empty()) {
            text.setString(L"путь к папке с сохранениями неуказон");
            text.setFillColor(sf::Color(202, 51, 62));//sf::Color(202, 51, 62)
            sprite.setTextureRect(sf::Rect{ 0, 576, 512, 32 });
        }
        else {
            text.setString(str);
            text.setFillColor(sf::Color::White);//sf::Color(45, 170, 74)
            sprite.setTextureRect(sf::Rect{ 0, 544, 512, 32 });
        }
    }

    inline void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        G_WINDOW.draw(text);
    }

    inline bool isClicked(const sf::Vector2f& mouseButton) const { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }

    inline bool isClickedButton(const sf::Vector2f& mouseButton) {
        const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        return mouseButton2.x >= 480;
    }
};