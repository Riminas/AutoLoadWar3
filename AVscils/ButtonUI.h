#pragma once
#include <SFML/Graphics.hpp>
#include "ConfigMaps.h"

class ButtonUI
{
public:
    static constexpr float SPRITE_SCALE = 0.15625f;  // 1/6.4

    // Объединяем методы установки позиции в один с перегрузкой
    void setPosition1(float x, float y) {
        sprite.setPosition(x, y);
    }

    void setPosition2(const sf::Vector2f& position) {
        sprite.setPosition(position);
    }

    // Упрощаем геттеры позиции
    sf::Vector2f getPosition() const { 
        return sprite.getPosition(); 
    }

    // Остальные методы оптимизированы путем передачи по константной ссылке
    void draw(sf::RenderWindow& window) const {
        window.draw(sprite);
    }

    void initialize(const sf::IntRect& intRect, const sf::Texture& texture) {
        sprite.setTexture(texture);
        sprite.setTextureRect(intRect);
        sprite.setScale(SPRITE_SCALE, SPRITE_SCALE);
    }

    bool isClicked(const sf::Vector2f& mousePosition, bool isVisible = true) const {
        return (isVisible && sprite.getGlobalBounds().contains(mousePosition));
    }

private:
    sf::Sprite sprite;
};
