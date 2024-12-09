#pragma once
#include <SFML/Graphics.hpp>
#include "StringConvector.h"
#include "FontSelector.h"

// Структура для хранения данных
class CommandsUI_0 {
private:
    sf::Sprite sprite;
    sf::Text text; // Основной текст
    bool m_Value{ 0 };
public:
    // Конструктор для инициализации данных
    void initialize(const bool t_Value, const std::string& str, const sf::Texture& texture, const bool isConvectorStr = true)
    {
        m_Value = t_Value;

        sprite.setTexture(texture);
        updateSpriteIsChecBox(m_Value);

        // Инициализация основного текста
        FontSelector().getFontForText(text);
        if(isConvectorStr) text.setString(StringConvector().utf8_to_utf16(str));
        else text.setString(str);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(14);
    }

    inline void updateSpriteIsChecBox(const bool val) {
        if (val) sprite.setTextureRect(sf::Rect{ 0, 64, 512, 32 });
        else sprite.setTextureRect(sf::Rect{ 0, 32, 512, 32 });
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

    inline bool getValue() const { return m_Value; }
    
    inline bool isClicked(const sf::Vector2f& mouseButton) const { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }

    inline bool isClickedCheckBox(const sf::Vector2f& mouseButton) {
        const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        if (mouseButton2.x >= 445 && mouseButton2.x < 490) {
            m_Value = (bool)!m_Value;
            updateSpriteIsChecBox(m_Value);
            return true;
        }
        return false;
    }
};