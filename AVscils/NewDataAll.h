#pragma once
#include <SFML/Graphics.hpp>
#include "DataWarcraft.h"
#include "DataMaps.h"
#include "DataAll.h"

class NewDataAll
{
public:
    NewDataAll();

    void newMaps();
    bool newWarcraft(const HWND& hWndWindow);
private:
    int PutSaveCode(const std::wstring& path);
    std::wstring removePath(const std::wstring& filePath);
    bool PathDirectorySaveCode(const std::wstring& path);
    bool loadDatFail(const std::wstring& fullPath);

    
    
};