#pragma once
#include <SFML/Graphics.hpp>
#include "Global.h"
#include <string>
#include "StringConvector.h"

// Структура для хранения данных
class OptionsUI {
private:
    sf::Sprite sprite;
    sf::Text text; // Основной текст
    bool m_Value{ false };
public:

    static constexpr float TEXT_X_OFFSET = 20.0f;
    static constexpr float TEXT_Y_OFFSET = 7.0f;
    static constexpr float CHECKBOX_X_MIN = 415.0f;
    static constexpr float CHECKBOX_X_MAX = 455.0f;
    static constexpr float MAX_TEXT_WIDTH = 380.0f;

    // Конструктор для инициализации данных
    void initialize(const bool t_Value, const std::wstring str, const sf::Texture& texture)
    {
        m_Value = t_Value;

        sprite.setTexture(texture);
        updateSpriteIsChecBox(m_Value);

        // Инициализация основного текста
        text.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        text.setString(str);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(14);
    }

    void updateSpriteIsChecBox(const bool val) {
        sprite.setTextureRect(val ? sf::Rect{ 0, 256, 512, 32 } : sf::Rect{ 0, 224, 512, 32 });
    }

    // Метод для установки позиции
    void setPosition(const float x, const float y) {
        StringConvector().adjustTextToFit(text, MAX_TEXT_WIDTH);
        text.setPosition(TEXT_X_OFFSET + x, TEXT_Y_OFFSET + y);
        sprite.setPosition(x, y);
    }

    void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        G_WINDOW.draw(text);
    }

    bool getValue() const { return m_Value; }

    bool isClicked(const sf::Vector2f& mouseButton) const { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }
  
    bool isClickedCheckBox(const sf::Vector2f& mouseButton) {
        const float relativeX = mouseButton.x - sprite.getPosition().x;
        if (relativeX >= CHECKBOX_X_MIN && relativeX < CHECKBOX_X_MAX) {
            m_Value = !m_Value;
            updateSpriteIsChecBox(m_Value);
            return true;
        }
        return false;
    }
};