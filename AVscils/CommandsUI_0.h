#pragma once
#include <SFML/Graphics.hpp>
#include "StringConvector.h"


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
        //if(isConvectorStr) text.setString(sf::String::fromUtf16(str.begin(), str.end()));
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
        StringConvector().adjustTextToFit(text, 380); // 236 = 512 / 2 - 20
        text.setPosition(20 + x, 7 + y);
        sprite.setPosition(x, y);
    }

    inline void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        
        // Отрисовка каждого символа отдельно
        float xOffset = 20 + sprite.getPosition().x;
        float yPosition = 7 + sprite.getPosition().y;
        float currentX = xOffset;
        const sf::String& wstr = text.getString();
        for (std::size_t i = 0; i < wstr.getSize(); ++i) {
            sf::Uint32 unicode = wstr[i];
            sf::Font font = G_FONT.getFontForCharacter(unicode);

            sf::Text character;
            character.setFont(font);
            character.setString(text.getString()[i]); // Необходимо убедиться, что символ корректно преобразован
            character.setFillColor(text.getFillColor());
            character.setCharacterSize(text.getCharacterSize());
            character.setPosition(currentX, yPosition);

            // Получение ширины символа для обновления позиции следующего символа
            sf::FloatRect bounds = character.getLocalBounds();
            currentX += bounds.width;

            G_WINDOW.draw(character);
        }
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
    inline bool isClickedEdit(const sf::Vector2f& mouseButton) {
        //const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        return {mouseButton.x - sprite.getPosition().x < 370};
    }
};