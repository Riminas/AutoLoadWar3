#include "FontSelector.h"
#include "Global.h"

FontSelector::FontSelector() {
}

void FontSelector::getFontForText(sf::Text& text) {
    text.setFont(G_FONT_STANDART);
    /*static std::vector<sf::Font*> globalFonts{ &G_FONT_STANDART, &G_FONT_CHINESE, &G_FONT_KOREAN, &G_FONT_HINDI };
    std::unordered_set<sf::Uint32> uniqueChars(text.getString().begin(), text.getString().end());

    for (sf::Font* font : globalFonts) {
        bool supportsAll = true;
        for (auto character : uniqueChars) {
            if (!font->getGlyph(character, text.getCharacterSize(), false).textureRect.width) {
                supportsAll = false;
                break;
            }
        }
        if (supportsAll) {
            text.setFont(*font);
            return;
        }
    }

    text.setFont(G_FONT_STANDART);*/
}