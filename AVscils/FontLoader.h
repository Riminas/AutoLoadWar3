#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include <unordered_map>

class FontLoader
{
public:
    void loadPredefinedFonts() const;
    //sf::Font& getFont(const std::string& fontPath);

private:
    //std::string getSystemFontsPath();
};
//
//// Глобальные предустановленные шрифты
//extern sf::Font G_FONT_STANDART;
//extern sf::Font G_FONT_CHINESE;
//extern sf::Font G_FONT_KOREAN;
//extern sf::Font G_FONT_HINDI;