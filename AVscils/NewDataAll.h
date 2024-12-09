#pragma once
#include <SFML/Graphics.hpp>
#include "DataPath.h"
#include "DataMaps.h"
#include "DataAll.h"

class NewDataAll
{
public:
    NewDataAll();

    void newMaps(const bool& isNoDataCommands = false, const bool& isOptions = false);
    bool newWarcraft(const HWND& hWndWindow);
private:
    int PutSaveCode(const std::wstring& path);
    std::wstring removePath(const std::wstring& filePath);
    bool PathDirectorySaveCode(const std::wstring& path);
    bool loadDatFail(const std::wstring& fullPath);

    
    
};