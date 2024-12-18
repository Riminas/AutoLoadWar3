#pragma once
#include <Windows.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "ConfigMain.h"

class ConfigMainEngine {
public:
    ConfigMainEngine();
    //void initializeDefaultConfig();
    bool loadConfigMain();
    bool saveConfigMain() const;
private:
    const std::filesystem::path m_FilePath{ L"DataWarAssist/WarAssist.ini" };

    bool parseBool(const std::string& value);
    int parseInt(const std::string& value);
};

