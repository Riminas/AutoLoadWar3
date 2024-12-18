#pragma once
#include <string>
#include <wtypes.h>
#include <sfml/Graphics.hpp>
#include <array>



//   Warcraft III
class DataWarcraft
{
private:
    struct DataPath {
        bool initializeDataPath(const HWND hWndWindow);

        int versionWarcraft = 0; // 0-нет 1-(ref) 2-(stand) 

        HWND hWndWindowWar = NULL;
        std::wstring warPathDirectMaps;
        std::vector<std::wstring> warPathDirectSave{2};
    private:
        std::wstring openWarcraft3(const HWND hWndWindow);
    };

    struct DataRect {
        bool initializeDataRect(const HWND hWndWindow);

        sf::Vector2i position{ 0, 0 };
        sf::Vector2i size{ 0, 0 };
        sf::Vector2i center{ 0, 0 };
    };
public:
    bool initializeDataWarcraft(const HWND hWndWindow);

    DataPath m_DataPath;
    DataRect m_DataRect;
};


extern DataWarcraft G_DATA_WARCRAFT;
