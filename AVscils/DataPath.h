#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <wtypes.h>

//���������� �� ���������� Warcraft III
class DataPath
{
public:
    bool initializePaths(const HWND hWndWindow);

    int versionWarcraft = 0; // 0-��� 1-(ref) 2-(stand) 

    HWND hWndWindowWar = NULL;

    std::wstring warPathDirectMaps = L"\0";
    std::wstring warPathDirectSave = L"\0";
private:
    std::wstring openWarcraft3(const HWND hWndWindow);
};


extern DataPath G_DATA_PATH;
