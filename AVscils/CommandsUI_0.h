#pragma once
#include <SFML/Graphics.hpp>
#include "StringConvector.h"


// Структура для хранения данных
class CommandsUI_0 {
private:
    sf::Sprite sprite;
    sf::Text text; // Основной текст
    bool m_Value{ false };
    
    static constexpr float TEXT_X_OFFSET = 20.0f;
    static constexpr float TEXT_Y_OFFSET = 7.0f;
    static constexpr float CHECKBOX_X_MIN = 445.0f;
    static constexpr float CHECKBOX_X_MAX = 490.0f;
    static constexpr float MAX_TEXT_WIDTH = 380.0f;

public:
    // Конструктор для инициализации данных
    void initialize(const bool t_Value, const std::string str, const sf::Texture& texture, const bool isConvectorStr = true)
    {
        m_Value = t_Value;

        sprite.setTexture(texture);
        updateSpriteIsChecBox(m_Value);

        if(isConvectorStr) text.setString(StringConvector().utf8_to_utf16(str));
        else text.setString(str);

        text.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(14);
    }

    inline void updateSpriteIsChecBox(const bool val) {
        if (val) sprite.setTextureRect(sf::Rect{ 0, 64, 512, 32 });
        else sprite.setTextureRect(sf::Rect{ 0, 32, 512, 32 });
    }

    // Метод для установки позиции
    inline void setPosition(const float x, const float y) {
        StringConvector().adjustTextToFit(text, MAX_TEXT_WIDTH);
        text.setPosition(TEXT_X_OFFSET + x, TEXT_Y_OFFSET + y);
        sprite.setPosition(x, y);
    }

    inline void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        
        const float baseX = TEXT_X_OFFSET + sprite.getPosition().x;
        const float baseY = TEXT_Y_OFFSET + sprite.getPosition().y;
        float currentX = baseX;
        
        const sf::String& str = text.getString();
        sf::Text character;
        character.setFillColor(text.getFillColor());
        character.setCharacterSize(text.getCharacterSize());

        for (const auto& unicode : str) {
            character.setFont(G_FONT.getFontForCharacter(unicode));
            character.setString(unicode);
            character.setPosition(currentX, baseY);
            currentX += character.getLocalBounds().width;
            G_WINDOW.draw(character);
        }
    }

    inline bool getValue() const { return m_Value; }
    
    inline bool isClicked(const sf::Vector2f& mouseButton) const { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }

    inline bool isClickedCheckBox(const sf::Vector2f& mouseButton) {
        const float relativeX = mouseButton.x - sprite.getPosition().x;
        return (relativeX >= CHECKBOX_X_MIN && relativeX < CHECKBOX_X_MAX) ? (m_Value = !m_Value, updateSpriteIsChecBox(m_Value), true) : false;
    }

    inline bool isClickedEdit(const sf::Vector2f& mouseButton) const {
        return (mouseButton.x - sprite.getPosition().x) < MAX_TEXT_WIDTH;
    }
};