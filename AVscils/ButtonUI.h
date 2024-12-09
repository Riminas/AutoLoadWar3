#pragma once
#include <SFML/Graphics.hpp>
#include "ConfigMaps.h"

class ButtonUI
{
public:
    // Рисует квадрат и текст на окне
    inline void draw(sf::RenderWindow& G_WINDOW) const {
        G_WINDOW.draw(sprite);
    }

    inline void initialize(const sf::IntRect& intRect, const sf::Texture& texture) {
        sprite.setTexture(texture);
        sprite.setTextureRect(intRect);
        sprite.setScale(0.15625f, 0.15625f);
    }

    // Установка позиции кнопки
    inline void setPosition1(float x, float y) {
        sprite.setPosition(x, y);
    }

    inline void setPosition2(sf::Vector2f  positionButton) {
        sprite.setPosition(positionButton);
    }

    // Проверка попадания координат мыши в квадрат и обновление значения
    inline bool isClicked(sf::Vector2f  mouseButton, const bool isVisible = true) {
        return (isVisible && sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y));
    }
    
    inline const float getPositionX() const { return sprite.getPosition().x; }
    inline const float getPositionY() const { return sprite.getPosition().y; }
private:
    sf::Sprite sprite;
};
