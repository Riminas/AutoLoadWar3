#pragma once
#include <string>

#include <fstream>
#include <filesystem>
#include "Windows.h"
#include "ConfigMaps.h"


class ConfigMapsEngine {
public:
    ConfigMapsEngine(const std::wstring& nameMaps);
    void initializeDefaultConfig();
    bool loadConfigMaps();
    bool saveConfigMaps();
    void openConfigMaps();

    void startGameCoutCmd(const HWND& hWndWindow);
private:
    std::wstring m_FilePathFull{ L"\0" };
    const std::wstring m_FilePath1 = L"DataWarAssist/DataMaps";
    const std::wstring m_FilePath2 = L"\\Path.ini";
    std::wstring trim(const std::wstring& str);

    bool parseBool(const std::string& value);
    int parseInt(const std::string& value);
};
