#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <array>
#include "LogError.h"

// Перечисление для идентификации шрифтов по категориям
enum class FontType {
    LatinCyrillic,
    Chinese,
    Korean,
    Count // Для указания количества типов
};

// Класс для управления подстановкой шрифта (Font Fallback)
class FontFallback {
public:
    FontFallback() {
        char* windir = nullptr;
        size_t len = 0;
        if (_dupenv_s(&windir, &len, "WINDIR") != 0 || windir == nullptr) {
            LogError().logError("Не удалось получить переменную окружения WINDIR");
            free(windir);
            exit(9);
        }
        std::string fontDirectory = std::filesystem::path(windir).string() + "\\Fonts\\";
        free(windir);

        sf::Font font;
        // Латиница и кириллица (Arial) 
        if (!font.loadFromFile(fontDirectory + "segoeui.ttf")) { 
            LogError().logError("Не удалось загрузить шрифт: arial.ttf");
        }
        else { 
            fonts[static_cast<size_t>(FontType::LatinCyrillic)] = font; 
        }
        // Упрощенный китайский (SimSun)
        if (!font.loadFromFile(fontDirectory + "simsun.ttc")) {
            LogError().logError("Не удалось загрузить шрифт: simsun.ttc");
        }
        else { 
            fonts[static_cast<size_t>(FontType::Chinese)] = font; 
        }
        // Корейский (Malgun Gothic)
        if (!font.loadFromFile(fontDirectory + "malgun.ttf")) { 
            LogError().logError("Не удалось загрузить шрифт: malgun.ttf");
        }
        else { 
            fonts[static_cast<size_t>(FontType::Korean)] = font; 
        }
    }

    // Метод для определения нужного шрифта по символу
    sf::Font getFontForCharacter(char32_t character) {
        // Определяем, к какой категории относится символ (латиница, кириллица, китайский, корейский)
        if (isLatinOrCyrillic(character)) {
            return fonts[static_cast<size_t>(FontType::LatinCyrillic)];
        }
        else if (isChinese(character)) {
            return fonts[static_cast<size_t>(FontType::Chinese)];
        }
        else if (isKorean(character)) {
            return fonts[static_cast<size_t>(FontType::Korean)];
        }
        else {
            // По умолчанию возвращаем латинский шрифт, если не найден подходящий
            return fonts[static_cast<size_t>(FontType::LatinCyrillic)];
        }
    }

    sf::Font getFontForString(std::u32string text) {
        bool isLatinCyrillicBool{ true },
            isChineseBool{ true },
            isKoreanBool{ true };

        for (const char32_t& character : text) {
            if (isLatinCyrillicBool && !isLatinOrCyrillic(character)) { isLatinCyrillicBool = false; }
            else if (isKoreanBool && !isKorean(character)) { isKoreanBool = false; }
            else if (isChineseBool && !isChinese(character)) { isChineseBool = false; }
        }

        if (isLatinCyrillicBool) return fonts[static_cast<size_t>(FontType::LatinCyrillic)];
        else if (isKoreanBool) return fonts[static_cast<size_t>(FontType::Korean)];
        else if(isChineseBool) return fonts[static_cast<size_t>(FontType::Chinese)];

        LogError().logError("Не удалось найти шрифт для текста: " + std::string(text.begin(), text.end()));
        return fonts[static_cast<size_t>(FontType::LatinCyrillic)];
    }

    std::array<sf::Font, 3> fonts; // Вектор для хранения шрифтов
private:
    // Проверка на принадлежность к диапазонам Unicode
    bool isLatinOrCyrillic(char32_t character) const {
        return (character >= 0x0020 && character <= 0x007F) || // Латиница (ASCII)
            (character >= 0x0400 && character <= 0x04FF);   // Кириллица (основной диапазон)
    }

    bool isChinese(char32_t character) const {
        return (character >= 0x4E00 && character <= 0x9FFF); // Китайские иероглифы (CJK Unified Ideographs)
    }

    bool isKorean(char32_t character) const {
        return (character >= 0xAC00 && character <= 0xD7AF); // Корейские слоги (Hangul Syllables)
    }

};
