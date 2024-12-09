#include <windows.h>
#include <fstream>

#include "LogError.h"
#include "FontLoader.h"
#include "Global.h"
#include "ConfigMain.h"
//
//sf::Font G_FONT_STANDART;
//sf::Font G_FONT_CHINESE;
//sf::Font G_FONT_KOREAN;
//sf::Font G_FONT_HINDI;

void FontLoader::loadPredefinedFonts() const {
    //std::string fontPath = getSystemFontsPath();

    if (!G_FONT_STANDART.loadFromFile("DataAutoLoad\\ArialUnicodeMS.ttf")) {
        if (G_CONFIG_MAIN.optionsConfig.writeLogs)
            LogError().logWarning("Не удалось загрузить шрифт: DataAutoLoad\\ArialUnicodeMS.ttf");
        MessageBox(NULL, L"Не удалось загрузить шрифт", L"Ошибка", MB_OK | MB_ICONEXCLAMATION);
    }

    /*struct FontEntry {
        sf::Font& font;
        std::string fileName;
    };

    std::vector<FontEntry> fontsToLoad = {
        { G_FONT_STANDART, fontPath + "segoeui.ttf" },
        { G_FONT_CHINESE, fontPath + "msyh.ttf" },
        { G_FONT_KOREAN, fontPath + "malgun.ttf" },
        { G_FONT_HINDI, fontPath + "nirmala.ttf" }
    };

    for (auto& entry : fontsToLoad) {
        if (!entry.font.loadFromFile(entry.fileName)) {
            LogError().logError1("Не удалось загрузить шрифт: " + entry.fileName);
            if (showWarnings) {
                MessageBox(NULL, L"Не удалось загрузить шрифт", L"Ошибка", MB_OK | MB_ICONEXCLAMATION);
            }
        }
    }*/
}

//std::string FontLoader::getSystemFontsPath() {
//    char windowsPath[MAX_PATH];
//    if (GetWindowsDirectoryA(windowsPath, MAX_PATH)) {
//        return std::string(windowsPath) + "\\Fonts\\";
//    }
//    return "C:\\Windows\\Fonts\\";
//}