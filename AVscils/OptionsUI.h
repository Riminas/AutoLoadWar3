﻿#pragma once
#include <SFML/Graphics.hpp>
#include "Global.h"
#include <string>
#include "StringConvector.h"

// Структура для хранения данных
class OptionsUI {
private:
    sf::Sprite sprite;
    sf::Text text; // Основной текст
    bool m_Value{ 0 };
public:
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

    inline void updateSpriteIsChecBox(const bool val) {
        if (val) sprite.setTextureRect(sf::Rect{ 0, 256, 512, 32 });
        else sprite.setTextureRect(sf::Rect{ 0, 224, 512, 32 });
    }

    // Метод для установки позиции
    inline void setPosition(const float x, const float y) {
        StringConvector().adjustTextToFit(text, 380); // 236 = 512 / 2 - 20
        text.setPosition(20 + x, 7 + y);
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
        if (mouseButton2.x >= 415 && mouseButton2.x < 455) {
            m_Value = (bool)!m_Value;
            updateSpriteIsChecBox(m_Value);
            return true;
        }
        return false;
    }
};