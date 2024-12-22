#pragma once
#include "StringConvector.h"

#include <unordered_map>
// Структура для хранения данных
class CommandsUI_1 {//один шаблон и его поочереди изменять и выводить? заместо 9 копий
private:
    sf::Sprite sprite;// чекбоксы будут одним целым спрайтом но прийдтся делать дополнительные изображения(6 на все случии с CommfndsUI_1)(1312 отдельные боксы 684 слитные боксы )
    sf::Text text; // Основной текст может получится записать все строки в один такст, но насколько будет бысткее 9 строк с одной строкой(9 text > 1 text + сложение строк и невожможность потом добавить редактировани команд)
    uint8_t m_TemStatus = 0;//0(false+false), 1(true+false), 2(false+true), 3(true, true)
    bool m_IsVisible{ false };
    bool m_IsStart{ false };
    
    // Добавляем константы для улучшения читаемости
    static constexpr float TEXT_OFFSET_X = 20.0f;
    static constexpr float TEXT_OFFSET_Y = 7.0f;
    static constexpr float CHECKBOX1_X = 375.0f;
    static constexpr float CHECKBOX2_X = 445.0f;
    static constexpr float CHECKBOX_WIDTH = 45.0f;
    static constexpr float MAX_TEXT_WIDTH = 380.0f;

public:
    // Конструктор для инициализации данных
    void initialize(const bool t_isVisible, const bool t_isStart, const std::string& str, const sf::Texture& texture)
    {
        m_IsVisible = t_isVisible;
        m_IsStart = t_isStart;
        updateSpriteIsChecBoxBool(m_IsVisible, m_IsStart);

        sprite.setTexture(texture);

        // Инициализация основного текста
        text.setString(StringConvector().utf8_to_utf16(str));
        text.setFont(G_FONT.fonts[static_cast<size_t>(FontType::LatinCyrillic)]);
        text.setFillColor(sf::Color::White);
        text.setCharacterSize(14); // Размер шрифта относительно квадрата
    }

    // Метод для установки позиции
    void setPosition(const float x, const float y) {
        // Корректировка текста для помещения в левую половину окна
        StringConvector().adjustTextToFit(text, MAX_TEXT_WIDTH);

        text.setPosition(TEXT_OFFSET_X + x, TEXT_OFFSET_Y + y);
        sprite.setPosition(x, y);
    }

    void draw(sf::RenderWindow& G_WINDOW) {
        G_WINDOW.draw(sprite);
        
        const float baseX = TEXT_OFFSET_X + sprite.getPosition().x;
        const float baseY = TEXT_OFFSET_Y + sprite.getPosition().y;
        float currentX = baseX;

        sf::Text character;
        character.setFillColor(text.getFillColor());
        character.setCharacterSize(text.getCharacterSize());

        const sf::String& wstr = text.getString();
        for (std::size_t i = 0; i < wstr.getSize(); ++i) {
            sf::Uint32 unicode = wstr[i];
            sf::Font font = G_FONT.getFontForCharacter(unicode);

            character.setFont(font);
            character.setString(text.getString()[i]); // Необходимо убедиться, что символ корректно преобразован
            character.setPosition(currentX, baseY);

            // Получение ширины символа для обновления позиции следующего символа
            sf::FloatRect bounds = character.getLocalBounds();
            currentX += bounds.width;

            G_WINDOW.draw(character);
        }
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
        sprite.setTextureRect(sf::IntRect(0, 96 + m_TemStatus * 32, 512, 32));
    }

    inline void updateSpriteIsChecBoxBool(const bool isVisible, const bool isStart) {
        m_TemStatus = isVisible + isStart * 2;
        updateSpriteIsChecBox();
    }

    inline bool getValueBox1() const { return m_IsVisible; }
    inline bool getValueBox2() const { return m_IsStart; }

    inline bool isClicked(const sf::Vector2f& mouseButton) { return sprite.getGlobalBounds().contains(mouseButton.x, mouseButton.y); }

    inline bool isClickedCheckBox1(const sf::Vector2f& mouseButton) {
        const float relativeX = mouseButton.x - sprite.getPosition().x;
        if (relativeX >= CHECKBOX1_X && relativeX < CHECKBOX1_X + CHECKBOX_WIDTH) {
            m_IsVisible = !m_IsVisible;
            m_TemStatus = m_IsVisible + m_IsStart * 2;
            updateSpriteIsChecBox();
            return true;
        }
        return false;
    }
    inline bool isClickedCheckBox2(const sf::Vector2f& mouseButton) {
        const float relativeX = mouseButton.x - sprite.getPosition().x;
        if (relativeX >= CHECKBOX2_X && relativeX < CHECKBOX2_X + CHECKBOX_WIDTH) {
            m_IsStart = !m_IsStart;
            m_TemStatus = m_IsVisible + m_IsStart * 2;
            updateSpriteIsChecBox();
            return true;
        }
        return false;
    }    
    inline bool isClickedEdit(const sf::Vector2f& mouseButton) {
        return { mouseButton.x - sprite.getPosition().x < CHECKBOX1_X };
    }
};