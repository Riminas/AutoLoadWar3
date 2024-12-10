#pragma once
#include "StringConvector.h"
#include "FontSelector.h"
#include <unordered_map>
// Структура для хранения данных
class CommandsUI_1 {//один шаблон и его поочереди изменять и выводить? заместо 9 копий
private:
    sf::Sprite sprite;// чекбоксы будут одним целым спрайтом но прийдтся делать дополнительные изображения(6 на все случии с CommfndsUI_1)(1312 отдельные боксы 684 слитные боксы )
    sf::Text text; // Основной текст может получится записать все строки в один такст, но насколько будет бысткее 9 строк с одной строкой(9 text > 1 text + сложение строк и невожможность потом добавить редактировани команд)
    uint8_t m_TemStatus = 0;//0(false+false), 1(true+false), 2(false+true), 3(true, true)
    bool m_IsVisible{ false };
    bool m_IsStart{ false };
public:
    // Конструктор для инициализации данных
    void initialize(const bool t_isVisible, const bool t_isStart, const std::string& str, const sf::Texture& texture)
    {
        m_IsVisible = t_isVisible;
        m_IsStart = t_isStart;
        updateSpriteIsChecBoxBool(m_IsVisible, m_IsStart);

        sprite.setTexture(texture);

        // Инициализация основного текста
        FontSelector().getFontForText(text);
        text.setString(StringConvector().utf8_to_utf16(str));
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(14); // Размер шрифта относительно квадрата
    }

    // Метод для установки позиции
    void setPosition(const float x, const float y) {
        // Корректировка текста для помещения в левую половину окна
        StringConvector().adjustTextToFit(text, 380); // 236 = 512 / 2 - 20

        text.setPosition(20 + x, 4 + y);
        sprite.setPosition(x, y);
    }

    void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        G_WINDOW.draw(text);
    }

    //CheckBox1
    // 0 > 1 > 0        1
    // 2 > 3 > 2        1
    //
    //CheckBox2
    // 0 > 2 > 0        2
    // 1 > 3 > 1        2
    //
    inline void updateSpriteIsChecBox() {
        switch (m_TemStatus) {
        case 0: sprite.setTextureRect(sf::Rect(0, 96, 512, 32)); return;
        case 1: sprite.setTextureRect(sf::Rect(0, 128, 512, 32)); return;
        case 2: sprite.setTextureRect(sf::Rect(0, 160, 512, 32)); return;
        case 3: sprite.setTextureRect(sf::Rect(0, 192, 512, 32)); return;
        }
    }

    inline void updateSpriteIsChecBoxBool(const bool isVisible, const bool isStart) {
        m_TemStatus = isVisible + isStart * 2;
        updateSpriteIsChecBox();
    }

    inline bool getValueBox1() const { return m_IsVisible; }
    inline bool getValueBox2() const { return m_IsStart; }

    inline bool isClicked(const sf::Vector2f& mouseButton) { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }

    inline bool isClickedCheckBox1(const sf::Vector2f& mouseButton) {
        const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        if (mouseButton2.x >= 375 && mouseButton2.x < 420) {
            m_IsVisible = !m_IsVisible;
            updateSpriteIsChecBoxBool(m_IsVisible, m_IsStart);
            return true;
        }
        return false;
    }
    inline bool isClickedCheckBox2(const sf::Vector2f& mouseButton) {
        const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        if (mouseButton2.x >= 445 && mouseButton2.x < 490) {
            m_IsStart = !m_IsStart;
            updateSpriteIsChecBoxBool(m_IsVisible, m_IsStart);
            return true;
        }
        return false;
    }    
    inline bool isClickedEdit(const sf::Vector2f& mouseButton) {
        //const sf::Vector2f mouseButton2{ mouseButton.x - sprite.getPosition().x, mouseButton.y - sprite.getPosition().y };
        return { mouseButton.x - sprite.getPosition().x < 370 };
    }
};